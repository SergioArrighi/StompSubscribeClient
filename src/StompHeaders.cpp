#include <StompHeaders.h>

namespace Stomp {

void StompHeaders::append(std::string key, std::string value) {
  _headers.push_back({key, value});
}

void StompHeaders::append(StompHeader header) {
  _headers.push_back(header);
}

StompHeader StompHeaders::get(size_t index) const {
  if (index >= _headers.size()) {
    throw std::out_of_range("Index out of range");
  }
  return _headers.at(index);
}

size_t StompHeaders::size() const {
  return _headers.size();
}

std::string StompHeaders::getValue(std::string key) {
  for (StompHeader header : _headers) {
    if (header.key == key)
      return header.value;
  }
  throw std::runtime_error("STOMP header not found: " + key);
}
}