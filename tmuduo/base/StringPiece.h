#ifndef STRINGPIECE_H
#define STRINGPIECE_H

#include <string_view>
#include "tmuduo/base/Types.h"

namespace tmuduo {

class StringArg {
 public:
  StringArg(const char* str) : str_(str) {
  }

  StringArg(const std::string& str) : str_(str.c_str()) {
  }

  const char* c_str() const {
    return str_;
  }

 private:
  const char* str_;
};

using StringPiece = std::string_view;

}  // namespace tmuduo

#endif  //  STRINGPIECE_H