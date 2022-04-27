#include "tmuduo/net/EventLoop.h"

#include <sys/eventfd.h>
#include <poll.h>

#include "tmuduo/base/Logging.h"
#include "tmuduo//net/Channel.h"
#include "tmuduo/net/Poller.h"
#include "tmuduo/net/TimerQueue.h"

using namespace tmuduo::net;

namespace {
__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10 * 1000;

int createEventfd()
{
  int eventfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (eventfd < 0)
  {
    LOG_SYSERR << "Failed in eventfd";
    abort();
  }
  return eventfd;
}

}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
  return t_loopInThisThread;
}

EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    eventHandling_(false),
    callingPendingFunctors_(false),
    threadId_(CurrentThread::tid()),
    poller_(Poller::newDefaultPoller(this)),
    timerQueue_(new TimerQueue(this)),
    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this, wakeupFd_))
{
  LOG_DEBUG << "EventLoop created at [" << this
            << "] in thread " << threadId_;
  if (t_loopInThisThread)
  {
    LOG_FATAL << "Another EventLoop " << t_loopInThisThread
              << " exists in this thread " << threadId_;
  }  
  else  
  {
    t_loopInThisThread = this;
  }
  wakeupChannel_->setReadCallback(
    std::bind(&EventLoop::readWakeupFd, this)
  );
  wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
  wakeupChannel_->disableAll();
  wakeupChannel_->remove();
  ::close(wakeupFd_);
  t_loopInThisThread = nullptr;
}

// 只能在创建线程中调用
void EventLoop::loop()
{
  assert(!looping_);
  assertInLoopThread();
  looping_ = (true);
  LOG_TRACE << "EventLoop at[" << this << "] starts looping";

  while (!quit_)
  {
    activeChannels_.clear();
    pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
    // debug
    if (Logger::logLevel() <= Logger::TRACE)
    {
      printActiveChannels();
    }
    eventHandling_ = (true);
    for (const auto channel : activeChannels_)
    {
      currentActiveChannel_ = channel;
      channel->handleEvent(pollReturnTime_);
    }
    currentActiveChannel_ = nullptr;
    eventHandling_ = (false);
    doPendingFunctors();
  }
  
  LOG_TRACE << "EventLoop at[" << this << "] stops looping";
  looping_ = (false);
}

void EventLoop::quit()
{
  quit_ = true;
  if (!isInLoopThread())
  {
    wakeup();
  }
}

void EventLoop::runInLoop(Functor cb)
{
  if (isInLoopThread())
  {
    cb();
  }
  else  
  {
    queueInLoop(std::move(cb));
  }
}

void EventLoop::queueInLoop(Functor cb)
{
  {
    std::scoped_lock<std::mutex> lock(mutex_);
    pendingFunctors_.push_back(std::move(cb));
  }
  if (!isInLoopThread() || callingPendingFunctors_)
  {
    wakeup();
  }
}

void EventLoop::wakeup()
{
  uint64_t one = 1;
  ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one))
  {
    LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::readWakeupFd()
{
  uint64_t one = 1;
  ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one))
  {
    LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::doPendingFunctors()
{
// LOG_TRACE;
  std::vector<Functor> functors;
  callingPendingFunctors_ = (true);

  {
    std::scoped_lock<std::mutex> lock(mutex_);
    functors.swap(pendingFunctors_);
  }
// LOG_TRACE << functors.size();
  for (const auto& functor : functors)
  {
    functor();
  }

  callingPendingFunctors_ = (false);
}

void EventLoop::updateChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  if (eventHandling_)
  {
    assert(currentActiveChannel_ == channel || 
           std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    // bug here fixed
  }
  poller_->removeChannel(channel);
}

void EventLoop::abortNotInLoopThread()
{
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop at [" << this
            << "] was created in threadId_ = " << threadId_
            << ", current thread id = " <<  CurrentThread::tid();
}

TimerId EventLoop::runAt(Timestamp time, TimerCallback cb)
{
  return timerQueue_->addTimer(std::move(cb), std::move(time), 0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback cb)
{
  // LOG_INFO << cb.target_type().name();
  return runAt(
    addTime(Timestamp::now(), delay), std::move(cb)
  );
}

TimerId EventLoop::runEvery(double interval, TimerCallback cb)
{
  return timerQueue_->addTimer(std::move(cb),
                               addTime(Timestamp::now(), interval), 
                               interval);
}

void EventLoop::cancel(TimerId TimerId)
{
  timerQueue_->cancel(TimerId);
}

void EventLoop::printActiveChannels() const
{
  for (auto channel : activeChannels_)
  {
    LOG_TRACE << "{" << channel->reventsToString() << "} ";
  }
}