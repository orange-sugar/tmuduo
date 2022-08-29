#include "tmuduo/net/EventLoopThread.h"
#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"

using namespace tmuduo;
using namespace tmuduo::net;

EventLoopThread::EventLoopThread(ThreadInitCallback cb)
    : loop_(nullptr), exiting_(false), thread_(nullptr), mutex_(), cond_(),
      callback_(std::move(cb)) {
}

EventLoopThread::~EventLoopThread() {
  exiting_ = true;
  loop_->quit();
  thread_->join();
}

EventLoop* EventLoopThread::startLoop() {
  LOG_DEBUG;
  thread_ = std::make_unique<std::thread>(std::bind(&EventLoopThread::threadFunc, this));
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (loop_ == nullptr) {
      cond_.wait(lock);
    }
  }
  return loop_;
}

void EventLoopThread::threadFunc() {
  EventLoop loop;
  if (callback_) {
    callback_(&loop);
  }
  {
    std::scoped_lock<std::mutex> lock(mutex_);
    loop_ = &loop;
    cond_.notify_one();
  }
  loop.loop();
}