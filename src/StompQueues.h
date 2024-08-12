#ifndef StompQueues_h
#define StompQueues_h

#include <string>

namespace Stomp {

/**
 * Enumeration of the queue types
 */
typedef enum {
  CLASSIC,
  QUORUM,
  STREAM
} Stomp_QueueType_t;

class StompQueues {
  public:
    static std::string getType(Stomp_QueueType_t type);
};
}
#endif