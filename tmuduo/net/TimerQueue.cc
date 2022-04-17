#include "tmuduo/net/TimerQueue.h"

#include <limits.h>
#include <sys/timerfd.h>

#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/Timer.h"
#include "tmuduo/net/TimerId.h"

namespace tmuduo
{
namespace net
{

namespace detail
{
int createTimerfd()
{
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0)
  {
    LOG_SYSFATAL << "Failed in timerfd_create";
  }
  return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
  int64_t microseconds = when.microSecondsSinceEpoch() -
                         Timestamp::now().microSecondsSinceEpoch();
  if (microseconds < 100)
  {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<time_t>(
    (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}

void readTimerfd(int timerfd, Timestamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
  LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at "
            << now.toString();
  if (n != sizeof(howmany))
  {
    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
  }
}

void resetTimerfd(int timerfd, Timestamp expiration)
{
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof(newValue));
  bzero(&oldValue, sizeof(oldValue));
  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if(ret < 0)
  {
    LOG_SYSERR << "timerfd_settime";
  }
}

}

using namespace tmuduo::net;
using namespace tmuduo::detail;
using namespace tmuduo::net::detail;

TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(detail::createTimerfd()),
    timerfdChannel_(loop, timerfd_)
{
  // TODO  //callingExpiredTimers_(false)
  timerfdChannel_.setReadCallback(
    std::bind(&TimerQueue::handleRead, this)
  );
  timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
  ::close(timerfd_);
}

TimerId TimerQueue::addTimer(TimerCallback cb,
                             Timestamp when,
                             double interval)
{
  std::unique_ptr<Timer> timer = std::make_unique<Timer>(std::move(cb), when, interval);
  TimerId timerId(timer->sequence());
  // unique_ptr 不可按值传递
  // loop_->runInLoop(make_copyable_function(
  //   [&, _timer = std::move(timer)]() mutable {
  //     addTimerInLoop(std::move(_timer));
  //   }
  // )
  // );

  loop_->runInLoop(MakeCopyableFunction(
    [&, _timer = std::move(timer)]() mutable {
      addTimerInLoop(std::move(_timer));
    }
  ));
  return timerId;
}

void TimerQueue::addTimerInLoop(std::unique_ptr<Timer>&& timer) 
{
  loop_->assertInLoopThread();
  Timestamp when = timer->expiration();
  bool earliestChanged = timers_.empty() || when < timerEntries_.cbegin()->first;
  if (earliestChanged)
  {
    resetTimerfd(timerfd_, timer->expiration());
  }
  TimerId timerId(timer->sequence());
  timerEntries_.insert(std::make_pair(timer->expiration(), timerId));
  timers_[timerId] = std::move(timer);
}

void TimerQueue::cancel(TimerId timerId)
{
  cancelInLoop(timerId);
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
  loop_->assertInLoopThread();

  assert(timers_[timerId]);
  
  LOG_DEBUG << "Cancel Timer(seq: " << timerId.sequence() << ")";
  
  auto it = timers_.find(timerId);
  if (it != timers_.end())
  {
    auto timeEntry = std::make_pair(it->second->expiration(), timerId);
    timerEntries_.erase(timeEntry);
    timers_.erase(it);
  }
  // auto ohter = timerEntries_.find(it->second->expiration());
}

std::vector<TimerQueue::TimerEntry> TimerQueue::getExpired(Timestamp now)
{
  // assert(timers_.size() == timerEntries_.size());
  std::vector<TimerEntry> expired;
  TimerEntry sentry(now, TimerId(INT64_MAX));

  // 返回第一个未到时的Timer的迭代器
  auto end = timerEntries_.lower_bound(sentry);
  assert(end == timerEntries_.end() || now < end->first);
  std::copy(timerEntries_.begin(), end, std::back_inserter(expired));
  timerEntries_.erase(timerEntries_.begin(), end);

  return expired;
}

void TimerQueue::handleRead()
{
  loop_->assertInLoopThread();
  Timestamp now(Timestamp::now());
  readTimerfd(timerfd_, now);
  {
    std::vector<TimerEntry> expired = getExpired(now);

    for (const auto &[_, timerId] : expired)
    {
      assert(timers_.count(timerId) > 0);
      auto &timer = timers_[timerId];
      timer->run();

      if (timer->repeat())
      {
        timer->restart(Timestamp::now());
        timerEntries_.emplace(timer->expiration(), 
                              TimerId(timer->sequence()));
      }
      else  
      {
        timers_.erase(timerId);
      }
    }
  }

  Timestamp nextExpireTime;
  if (!timerEntries_.empty())
  {
    nextExpireTime = timerEntries_.begin()->first;
  }
  if (nextExpireTime.valid())
  {
    detail::resetTimerfd(timerfd_, nextExpireTime);
  }
}

}  // namespace tmuduo
}  // namespace net
