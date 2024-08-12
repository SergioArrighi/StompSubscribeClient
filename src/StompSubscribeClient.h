#ifndef StompSubscribe_h
#define StompSubscribe_h

#ifndef STOMP_MAX_SUBSCRIPTIONS
#define STOMP_MAX_SUBSCRIPTIONS 8
#endif

#include <Stomp.h>
#include <WebsocketsClient.h>

namespace Stomp {

class StompSubscribeClient {
  public:
    StompSubscribeClient(
      WebSocketsClient &wsClient,
      const char *host,
      const int port,
      const char *url,
      const unsigned int heartBeatInterval
    );
    ~StompSubscribeClient();

    void begin();
    // TODO implement WSS
    void beginSSL();
    void loop();
    int subscribe(
      const char *queue,
      Stomp_QueueType_t queueType,
      Stomp_AckMode_t ackType,
      StompMessageHandler handler
    );
    void unsubscribe(int subscription);
    void ack(StompCommand message);
    void nack(StompCommand message);
    void disconnect();
    void sendHeartBeat();
    void onConnect(StompStateHandler handler);
    void onDisconnect(StompStateHandler handler);
    void onReceipt(StompStateHandler handler);
    void onError(StompStateHandler handler);

  private:
    WebSocketsClient &_wsClient;
    const char *_host;
    const int _port;
    const char *_url;

    bool _handshakeComplete = false;

    long _id = 0;
    uint32_t _heartbeats = 0;
    uint32_t _commandCount = 0;
    Stomp_State_t _state = DISCONNECTED;
    StompSubscription _subscriptions[STOMP_MAX_SUBSCRIPTIONS];

    unsigned long _lastHeartBeatSent = 0;
    unsigned long _lastHeartBeatReceived = 0;
    unsigned int _heartBeatInterval = 0;

    StompStateHandler _connectHandler;
    StompStateHandler _disconnectHandler;
    StompStateHandler _receiptHandler;
    StompStateHandler _errorHandler;
    
    void _handleWebSocketEvent(WStype_t type, uint8_t * payload, size_t length);
    void _connectStomp();
    void _handleCommand(StompCommand command);
    void _handleConnected(StompCommand command);
    void _handleMessage(StompCommand message);
    void _handleReceipt(StompCommand command);
    void _handleError(StompCommand command);
    void _send(StompCommandName command, StompHeaders headers, std::string body[]);
};
}
#endif