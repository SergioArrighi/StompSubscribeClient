#include <stdexcept>
#include <StompQueues.h>

namespace Stomp {

std::string StompQueues::getType(Stomp_QueueType_t type) {
  switch (type) {
    case Stomp_QueueType_t::CLASSIC: return "classic";
    case Stomp_QueueType_t::QUORUM: return "quorum";
    case Stomp_QueueType_t::STREAM: return "stream";
    default: throw std::runtime_error("Unknown STOMP command");
  }
};
}