#ifndef StompHeaders_h
#define StompHeaders_h

#include <regex>
#include <string>
#include <vector>

namespace Stomp {

typedef struct {
    std::string key;
    std::string value;
} StompHeader;

class StompHeaders {
  public:  
    void append(std::string key, std::string value);
    void append(StompHeader header);
    StompHeader get(size_t index) const;
    std::string getValue(std::string key);
    size_t size() const;

  private:
    std::vector<StompHeader> _headers;

};
}

#endif