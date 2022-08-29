#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <assert.h>
#include <condition_variable>
#include <deque>
#include <mutex>
#include "tmuduo/base/noncopyable.h"

using std::condition_variable;
using std::deque;
using std::lock_guard;
using std::mutex;
using std::unique_lock;

namespace tmuduo {

template <typename T>
class BlockingQueue : noncopyable {
 public:
  BlockingQueue() {
  }
  void put(const T& x) {
    lock_guard<mutex> lock(mutex_);
    queue_.push_back(x);
    notEmpty_.notify_one();
  }

  void put(T&& x) {
    lock_guard<mutex> lock(mutex_);
    queue_.push_back(x);
    notEmpty_.notify_one();
  }

  T take() {
    unique_lock<mutex> lock(mutex_);
    // 避免虚假唤醒, 放一个循环判断
    while (queue_.empty()) {
      notEmpty_.wait(lock);
    }
    assert(!queue_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
    return front;
  }

  size_t size() const {
    lock_guard<mutex> lock(mutex_);
    return queue_.size();
  }

 private:
  mutable mutex mutex_;
  condition_variable notEmpty_;
  deque<T> queue_;
};

}  // namespace tmuduo

#endif
