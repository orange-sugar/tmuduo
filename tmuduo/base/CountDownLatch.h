#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "tmuduo/base/noncopyable.h"

#include <condition_variable>
#include <mutex>

using std::mutex;
using std::condition_variable;

namespace tmuduo
{

class CountDownLatch : noncopyable
{
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

} // namespace tmuduo

#endif