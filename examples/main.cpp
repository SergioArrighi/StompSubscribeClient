#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <StompSubscribeClient.h>

// WiFi credentials
const char* ssid = "<SSID>";
const char* password = "<PASSWORD>";

WebSocketsClient webSocket;

const char* queue = "<QUEUE>";
const char* host = "<HOST>";
const int port = 15674; // RabbitMQ default web STOMP port
const char* url = "<WS_URL>";
const unsigned int heartbeatInterval = 10000;

// STOMP client
Stomp::StompSubscribeClient stompClient(webSocket, host, port, url, heartbeatInterval);

// Example handler for connection state changes
void stateHandler(const Stomp::StompCommand message) {
  Serial.println("State change handler called");
  Serial.println(message.command.c_str());
  for (size_t i = 0; i < message.headers.size(); ++i) {
      Stomp::StompHeader header = message.headers.get(i);
      Serial.print(header.key.c_str());
      Serial.print(":");
      Serial.println(header.value.c_str());
  }
  Serial.println();
  Serial.print(message.body.c_str());
  Serial.println();
}

/*
Example of callback functions
// Example handler for incoming STOMP messages
Stomp::Stomp_Ack_t messageHandler(const Stomp::StompCommand message) {
  Serial.print("Received message: ");
  Serial.println(message.body.c_str());
  // Instruct the client to ack, nack or continue
  return Stomp::ACK;
}

void onConnected(const Stomp::StompCommand message) {
    Serial.println("Connected to STOMP server");

    // Subscribe to a queue
    char queue[] = "nfc-auth.success.192.168.1.1";
    stompClient.subscribe(queue, Stomp::AUTO, messageHandler);
}
*/

void setup() {
  Serial.begin(115200);
  while (!Serial);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi, IP address: " + WiFi.localIP().toString());

  stompClient.onConnect(stateHandler);
  stompClient.onDisconnect(stateHandler);
  stompClient.onReceipt(stateHandler);
  stompClient.onError(stateHandler);
  stompClient.begin();

  // Example with lambda callback functions
  stompClient.onConnect([](Stomp::StompCommand cmd) {
    Serial.println("Connected to STOMP server");
    // Subscribe to the queue
    stompClient.subscribe(queue, Stomp::STREAM, Stomp::CLIENT,
      [](const Stomp::StompCommand message) -> Stomp::Stomp_Ack_t {
        Serial.println("Received message: ");
        Serial.println(message.body.c_str());
        return Stomp::ACK;
      }
    );
  });

}

void loop() {
  stompClient.loop();
  webSocket.loop();
}