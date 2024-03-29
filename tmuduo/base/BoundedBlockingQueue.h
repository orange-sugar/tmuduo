#ifndef BOUNDEDBLOCKINGQUEUE_H
#define BOUNDEDBLOCKINGQUEUE_H

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
class BoundedBlockingQueue : noncopyable {
 public:
  explicit BoundedBlockingQueue(unsigned maxSize) : maxSize_(maxSize) {
  }
  void put(const T&& x) {
    unique_lock<mutex> lock(mutex_);
    while (queue_.size() == maxSize_) {
      notFull_.wait(lock);
    }
    assert(queue_.size() < maxSize_);
    queue_.push_back(std::move(x));
    notEmpty_.notify_one();
  }

  void put(const T& x) {
    unique_lock<mutex> lock(mutex_);
    while (queue_.size() == maxSize_) {
      notFull_.wait(lock);
    }
    assert(queue_.size() < maxSize_);
    queue_.push_back(x);
    notEmpty_.notify_one();
  }

  T take() {
    unique_lock<mutex> lock(mutex_);
    while (queue_.empty()) {
      notEmpty_.wait(lock);
    }
    T front(queue_.front());
    queue_.pop_front();
    notFull_.notify_one();
    return front;
  }

  bool empty() const {
    lock_guard<mutex> lock(mutex_);
    return queue_.empty();
  }

  bool full() const {
    lock_guard<mutex> lock(mutex_);
    return queue_.size() == maxSize_;
  }

  size_t size() const {
    lock_guard<mutex> lock(mutex_);
    return queue_.size();
  }

  size_t capacity() const {
    lock_guard<mutex> lock(mutex_);
    return queue_.capacity();
  }

 private:
  const unsigned maxSize_;
  mutable mutex mutex_;
  condition_variable notEmpty_;
  condition_variable notFull_;
  deque<T> queue_;
};

}  // namespace tmuduo

#endif
