#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace tmuduo {

class noncopyable {
 public:
  noncopyable(const noncopyable&) = delete;
  void operator=(const noncopyable&) = delete;

 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

}  // namespace tmuduo

#endif  //  NONCOPYABLE_H