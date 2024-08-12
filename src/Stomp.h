#ifndef STOMP_H
#define STOMP_H

#ifndef STOMP_MAX_COMMAND_HEADERS
#define STOMP_MAX_COMMAND_HEADERS 16
#endif

#include <StompHeaders.h>
#include <StompCommands.h>
#include <StompQueues.h>

namespace Stomp {

/**
 * Enumeration of the acknowledgement mode specified by a client when it subscribes to a topic
 */
typedef enum {
  AUTO,
  CLIENT,
  CLIENT_INDIVIDUAL
} Stomp_AckMode_t;

/**
 * Enumeration of the values to be returned by a client message handler
 * ACK - return an ACK to the server
 * NACK - return a NACK to the server
 * CONTINUE - return nothing to the server. Used when the acknowledgement mode is CLIENT to allow batching of ACK/NACKs
 */
typedef enum {
  ACK,
  NACK,
  CONTINUE
} Stomp_Ack_t;

/**
 * The current state of the STOMP connection
 */
typedef enum {
  OPENING,
  CONNECTED,
  DISCONNECTING,
  DISCONNECTED
} Stomp_State_t;

/**
 * Signature of functions which handle incoming MESSAGEs
 */
typedef Stomp_Ack_t (*StompMessageHandler)(const StompCommand message);

/**
 * Signature of functions which handle other types of incoming command
 */
typedef void (*StompStateHandler)(const StompCommand message);

typedef struct {
  long id;
  StompMessageHandler messageHandler;
} StompSubscription;
}

#endif
