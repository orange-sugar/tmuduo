#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include <condition_variable>
#include <mutex>
#include "tmuduo/base/noncopyable.h"

using std::condition_variable;
using std::mutex;

namespace tmuduo {

class CountDownLatch : noncopyable {
 public:
  explicit CountDownLatch(int count);

  void wait();

  void countDown();

  int getCount() const;

 private:
  mutable mutex mutex_;
  condition_variable condition_;
  int count_;
};

}  // namespace tmuduo

#endif
