#ifndef TMUDUO_NET_EVENTLOOPTHREAD_H
#define TMUDUO_NET_EVENTLOOPTHREAD_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include "tmuduo//net/Callbacks.h"
#include "tmuduo/base/noncopyable.h"

namespace tmuduo {
namespace net {

class EventLoop;

class EventLoopThread : noncopyable {
 public:
  using ThreadInitCallback = std::function<void(EventLoop*)>;

  EventLoopThread(ThreadInitCallback cb = ThreadInitCallback());
  ~EventLoopThread();

  EventLoop* startLoop();

 private:
  void threadFunc();

  EventLoop* loop_;
  bool exiting_;
  std::unique_ptr<std::thread> thread_;
  std::mutex mutex_;
  std::condition_variable cond_;
  ThreadInitCallback callback_;
};

}  // namespace net
}  // namespace tmuduo

#endif  // TMUDUO_NET_EVENTLOOPTHREAD_H