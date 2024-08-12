#ifndef StompCommands_h
#define StompCommands_h

#include <string>
#include <StompHeaders.h>

namespace Stomp {

typedef struct {
  std::string command;
  StompHeaders headers;
  std::string body;

} StompCommand;

enum class StompCommandName {
  // Commands sent by the client
  CONNECT,
  SEND,
  SUBSCRIBE,
  UNSUBSCRIBE,
  BEGIN,
  COMMIT,
  ABORT,
  ACK,
  NACK,
  DISCONNECT,

  // Commands received from the server
  CONNECTED,
  MESSAGE,
  RECEIPT,
  ERROR
};

class StompCommands {
  public:
    static bool isCommand(std::string data);
    static std::string toString(StompCommandName command);
    static StompCommand parse(std::string data);
};
}

#endif