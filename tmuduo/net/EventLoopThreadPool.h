#ifndef TMUDUO_NET_EVENTLOOPTHREADPOOL_H
#define TMUDUO_NET_EVENTLOOPTHREADPOOL_H

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
#include "tmuduo/base/noncopyable.h"

namespace tmuduo {
namespace net {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable {
 public:
  using ThreadInitCallback = std::function<void(EventLoop*)>;
  EventLoopThreadPool(EventLoop* loop);
  ~EventLoopThreadPool();

  void setThreadNum(int numThreads) {
    numThreads_ = numThreads;
  }
  void start(const ThreadInitCallback& cb = ThreadInitCallback());
  EventLoop* getNextLoop();

 private:
  EventLoop* baseLoop_;
  bool started_;
  int numThreads_;
  int next_;
  std::vector<std::unique_ptr<EventLoopThread>> threads_;
  std::vector<EventLoop*> loops_;
};

}  //  namespace net
}  //  namespace tmuduo

#endif  // TMUDUO_NET_EVENTLOOPTHREADPOOL_H