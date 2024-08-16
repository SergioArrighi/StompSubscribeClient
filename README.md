# StompSubscribeClient

A RabbitMQ STOMP 1.2 WebSocket client with connection, heart-beat and subscription support.

This library is heavily inspired by [ukmaker/StompClient](https://github.com/ukmaker/StompClient)

## Client initialisation

	StompSubscribeClient(
		WebSocketsClient &wsClient,
		const char *host,
		const int port,
		const char *url,
		const unsigned int heartBeatInterval
	);

Initialising an instance of the client requires the following parameters:

  

- A WebSocketsClient instance
- The host of the RabbitMQ server (ex.: 192.168.1.23 or www.example.com)
- The port of the RabbitMQ server
- The url of the RabbitMQ server (defaults to **/ws**)
- The STOMP heart beat interval

> The websockets client is received from the caller so it can be ideally shared with other services. Currently though, no synchronisation is enforced, so be careful if the websockets client is not fully dedicated.

> **The caller also needs to call the loop() function on the websockets client instance provided to the STOMP client**.


## Features

This library provides a model and several callback hooks to interact with STOMP 1.2. It is conceived for a readonly use case, as it only implements queue subscription.

  

### Protocol hooks

- stompClient.onConnect(stateHandler);
- stompClient.onDisconnect(stateHandler);
- stompClient.onReceipt(stateHandler);
- stompClient.onError(stateHandler);

Where stateHandler is a function with signature:

	typedef void (*StompStateHandler)(const StompCommand message);

  

### Subscribe hook

	int subscribe(
		const char* queue,
		Stomp_QueueType_t queueType,
		Stomp_AckMode_t ackType,
		StompMessageHandler handler,
		IContext* context
	);

The client supports a maximum number of concurrent subscriptions (STOMP_MAX_SUBSCRIPTIONS).

-  ***queue*** is the handle of the RabbitMQ target queue (ex.: this.is.my.queue).
-  ***queueType*** is its type (CLASSIC, QUORUM, STREAM).
-  ***ackMode*** represents the acknowledgement strategy (AUTO, CLIENT, CLIENT_INDIVIDUAL).
-  ***handler*** is a callback function respecting the **StompMessageHandler** signature
- ***context*** is a pointer to an instance of an implementation of Stomp::IContext and it is used to relay the needed data to the callback in the case it is a static function. It can be *nullptr* if no additional data is needed. The caller will need to statically cast it to the desired type.

  
StompMessageHandler signature:

		typedef Stomp_Ack_t (*StompMessageHandler)(const StompCommand message, IContext* context);

  

> The subscribe hook should be used in the handler provided to **onConnect**

  

### Example

An example is provided in the dedicated folder.

Replace the variable placeholders with the appropriate data.

The example shows how to use handlers that are explicitly defined or lambda functions.