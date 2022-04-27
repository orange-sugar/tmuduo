#include "tmuduo/net/EventLoopThreadPool.h"

#include <assert.h>

#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/EventLoopThread.h"

using namespace tmuduo::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
  : baseLoop_(baseLoop),
    started_(false),
    numThreads_(0),
    next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{ }

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
  assert(!started_);
  baseLoop_->assertInLoopThread();

  started_ = true;

  for (int i = 0; i < numThreads_; ++i)
  {
    // auto t = std::make_unique<EventLoopThread>(cb);
    threads_.emplace_back(std::make_unique<EventLoopThread>(cb));
    loops_.push_back(threads_[i]->startLoop());
  }

  if (numThreads_ == 0 && cb)
  {
    cb(baseLoop_);
  }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
  baseLoop_->assertInLoopThread();
  EventLoop* loop = baseLoop_;
  if (!loops_.empty())
  {
    loop = loops_[next_];
    ++next_;
    if (implicit_cast<size_t>(next_) >= loops_.size())
    {
      next_ = 0;
    }
  }
  return loop;
}