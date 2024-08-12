#include <random>
#include <StompSubscribeClient.h>

namespace Stomp {

StompSubscribeClient::StompSubscribeClient(
  WebSocketsClient &wsClient,
  const char *host,
  const int port,
  const char *url,
  const unsigned int heartBeatInterval
) : _wsClient(wsClient), _host(host), _port(port), _url(url), _heartBeatInterval(heartBeatInterval) {

  _wsClient.onEvent( [this] (WStype_t type, uint8_t * payload, size_t length) {
    this->_handleWebSocketEvent(type, payload, length);
  } );

  for (int i = 0; i < STOMP_MAX_SUBSCRIPTIONS; i++) {
    _subscriptions[i].id = -1;
  }
}

void StompSubscribeClient::_handleWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  std::string text;
  StompCommand command;

  switch (type) {
    case WStype_DISCONNECTED:
      _state = DISCONNECTED;
      break;

    case WStype_CONNECTED:
      _connectStomp();
      break;

    case WStype_TEXT:
      _lastHeartBeatReceived = millis();
      text = std::string((char*) payload);
      if (!text.empty() && StompCommands::isCommand(text)) {
        command = StompCommands::parse(text);
        _handleCommand(command);
      }
      break;

    case WStype_BIN:
      break;
  }
}

StompSubscribeClient::~StompSubscribeClient() {
  disconnect();
  _wsClient.disconnect();
}

void StompSubscribeClient::begin() {
  _wsClient.begin(_host, _port, _url);
  _wsClient.setExtraHeaders();
  _lastHeartBeatSent = millis();
  _lastHeartBeatReceived = millis();
}

// TODO implement WSS
void StompSubscribeClient::beginSSL() {}

void StompSubscribeClient::loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - _lastHeartBeatSent >= _heartBeatInterval) {
    sendHeartBeat();
    _lastHeartBeatSent = currentMillis;
  }

  _wsClient.loop();
}

int StompSubscribeClient::subscribe(
  const char *queue,
  Stomp_QueueType_t queueType,
  Stomp_AckMode_t ackType,
  StompMessageHandler handler
) {
  // Scan for an unused subscription slot
  for (int i = 0; i < STOMP_MAX_SUBSCRIPTIONS; i++) {

    if (_subscriptions[i].id == -1) {

      _subscriptions[i].id = i;
      _subscriptions[i].messageHandler = handler;

      std::string ack;
      switch (ackType) {
        case AUTO:
          ack = "auto";
          break;
        case CLIENT:
          ack = "client";
          break;
        case CLIENT_INDIVIDUAL:
          ack = "client-individual";
          break;
      }
      
      StompHeaders stompHeaders;
      stompHeaders.append("id", "sub-" + std::to_string(i));
      stompHeaders.append("destination", std::string(queue));
      stompHeaders.append("ack", ack);
      stompHeaders.append("prefetch-count", "1");
      stompHeaders.append(
        "x-queue-type",
        StompQueues::getType(queueType)
      );
      _send(StompCommandName::SUBSCRIBE, stompHeaders, {});

      return i;
    }
  }
  return -1;
}

void StompSubscribeClient::unsubscribe(int subscription) {
  StompHeaders stompHeaders;
  stompHeaders.append("id", "sub-" + std::to_string(subscription));
  _send(StompCommandName::UNSUBSCRIBE, stompHeaders, {});

  _subscriptions[subscription].id = -1;
  _subscriptions[subscription].messageHandler = 0;
}

void StompSubscribeClient::ack(StompCommand message) {
  StompHeaders stompHeaders;
  stompHeaders.append("id", message.headers.getValue("ack"));
  _send(StompCommandName::ACK, stompHeaders, {});
}

void StompSubscribeClient::nack(StompCommand message) {
  StompHeaders stompHeaders;
  stompHeaders.append("id", message.headers.getValue("ack"));
  _send(StompCommandName::NACK, stompHeaders, {});
}

void StompSubscribeClient::disconnect() {
  StompHeaders stompHeaders;
  stompHeaders.append("receipt", std::to_string(_commandCount));
  _send(StompCommandName::DISCONNECT, stompHeaders, {});
}

void StompSubscribeClient::sendHeartBeat() {
  _wsClient.sendTXT("\n");
}

void StompSubscribeClient::onConnect(StompStateHandler handler) {
  _connectHandler = handler;
}

void StompSubscribeClient::onDisconnect(StompStateHandler handler) {
  _disconnectHandler = handler;
}

void StompSubscribeClient::onReceipt(StompStateHandler handler) {
  _receiptHandler = handler;
}

void StompSubscribeClient::onError(StompStateHandler handler) {
  _errorHandler = handler;
}

void StompSubscribeClient::_connectStomp() {
  if (_state != OPENING) {
    _state = OPENING;
    StompHeaders headers;
    headers.append("accept-version", "1.2");
    // TODO implement heart beat
    headers.append("heart-beat", std::to_string(_heartBeatInterval) + "," +
      std::to_string(_heartBeatInterval)
    );
    headers.append("login", "guest");
    headers.append("passcode", "guest");
    headers.append("host", "/");
    _send(StompCommandName::CONNECT, headers, {});
  }
}

void StompSubscribeClient::_handleCommand(StompCommand command) {
  if (command.command == StompCommands::toString(StompCommandName::CONNECTED))
    _handleConnected(command);
  else if (command.command == StompCommands::toString(StompCommandName::MESSAGE))
    _handleMessage(command);
  else if (command.command == StompCommands::toString(StompCommandName::RECEIPT))
    _handleReceipt(command);
  else if (command.command == StompCommands::toString(StompCommandName::ERROR))
    _handleError(command);
}

void StompSubscribeClient::_handleConnected(StompCommand command) {
  if (_state != CONNECTED) {
    _state = CONNECTED;
    if (_connectHandler)
      _connectHandler(command);
  }
}

void StompSubscribeClient::_handleMessage(StompCommand message) {
  std::string sub = message.headers.getValue("subscription");
  std::string toMatch = "sub-";
  if (sub.compare(0, toMatch.length(), toMatch) != 0)
    return;
  int id = std::stoi(sub.substr(4));

  std::string messageId = message.headers.getValue("message-id");

  StompSubscription *subscription = &_subscriptions[id];
  if (subscription->id != id)
    return;

  if (subscription->messageHandler) {
    StompMessageHandler callback = subscription->messageHandler;
    Stomp_Ack_t ackType = callback(message);
    switch (ackType) {
      case ACK:
        ack(message);
        break;

      case NACK:
        nack(message);
        break;

      case CONTINUE:
      default:
        break;
    }
  }
}

void StompSubscribeClient::_handleReceipt(StompCommand command) {

  if(_receiptHandler)
    _receiptHandler(command);

  if (_state == DISCONNECTING) {
    _state = DISCONNECTED;
    if (_disconnectHandler)
      _disconnectHandler(command);
  }
}

void StompSubscribeClient::_handleError(StompCommand command) {
  _state = DISCONNECTED;
  if (_errorHandler)
    _errorHandler(command);
  if (_disconnectHandler)
    _disconnectHandler(command);
}

void StompSubscribeClient::_send(
  StompCommandName command,
  StompHeaders headers,
  std::string body[]) {
  std::string msg = StompCommands::toString(command) + "\n";
  for(int i = 0; i < headers.size(); i++) {
    StompHeader h = headers.get(i);
    msg += h.key + ":" + h.value + "\n";
  }

  msg += "\n^@";

  _wsClient.sendTXT(msg.c_str(), msg.length() + 1);
  _lastHeartBeatSent = millis();
  _commandCount++;
}
}