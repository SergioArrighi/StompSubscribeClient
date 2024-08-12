#include <stdexcept>
#include <sstream>
#include <iostream>
#include <StompCommands.h>

namespace Stomp {

bool StompCommands::isCommand(std::string data) {
  return data.rfind("SUBSCRIBE", 0) == 0 || data.rfind("MESSAGE", 0) == 0 || 
    data.rfind("CONNECT", 0) == 0 || data.rfind("CONNECTED", 0) == 0 || 
    data.rfind("DISCONNECT", 0) == 0 || data.rfind("SEND", 0) == 0 ||
    data.rfind("RECEIPT", 0) == 0 || data.rfind("ERROR", 0) == 0;
}

std::string StompCommands::toString(StompCommandName command) {
  switch (command) {
    case StompCommandName::CONNECT: return "CONNECT";
    case StompCommandName::DISCONNECT: return "DISCONNECT";
    case StompCommandName::SUBSCRIBE: return "SUBSCRIBE";
    case StompCommandName::UNSUBSCRIBE: return "UNSUBSCRIBE";
    case StompCommandName::SEND: return "SEND";
    case StompCommandName::BEGIN: return "BEGIN";
    case StompCommandName::COMMIT: return "COMMIT";
    case StompCommandName::ABORT: return "ABORT";
    case StompCommandName::ACK: return "ACK";
    case StompCommandName::NACK: return "NACK";
    case StompCommandName::CONNECTED: return "CONNECTED";
    case StompCommandName::MESSAGE: return "MESSAGE";
    case StompCommandName::RECEIPT: return "RECEIPT";
    case StompCommandName::ERROR: return "ERROR";
    default: throw std::runtime_error("Unknown STOMP command");
  }
}

StompCommand StompCommands::parse(std::string data) {
  std::istringstream stream(data);
  std::string line;
  StompCommand command;

  // Extract the command
  if (!std::getline(stream, line)) {
      throw std::runtime_error("Failed to parse command");
  }
  command.command = line;

  // Extract headers
  while (std::getline(stream, line) && !line.empty()) {
      size_t pos = line.find(':');
      if (pos == std::string::npos) {
          throw std::runtime_error("Invalid header format");
      }
      std::string key = line.substr(0, pos);
      std::string value = line.substr(pos + 1);
      command.headers.append(std::move(key), std::move(value));
  }

  // Extract body
  std::string body;
  while (std::getline(stream, line)) {
      if (!body.empty()) {
          body += "\n";
      }
      body += line;
  }

  // Remove trailing null character if present
  if (!body.empty() && body.back() == '\0') {
      body.pop_back();
  }

  command.body = std::move(body);

  return command;
}
}