#ifndef TMUDUO_NET_TIMERQUEUE_H
#define TMUDUO_NET_TIMERQUEUE_H

#include <atomic>
#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>
#include "tmuduo/base/Timestamp.h"
#include "tmuduo/base/noncopyable.h"
#include "tmuduo/net/Callbacks.h"
#include "tmuduo/net/Channel.h"
#include "tmuduo/net/TimerId.h"

namespace tmuduo {
namespace net {

class EventLoop;
class Timer;
// class TimerId;

class TimerQueue : noncopyable {
 public:
  TimerQueue(EventLoop* loop);
  ~TimerQueue();

  TimerId addTimer(TimerCallback cb, Timestamp when, double interval);

  void cancel(TimerId timerId);

 private:
  using TimerMap = std::unordered_map<TimerId, std::unique_ptr<Timer>>;
  using TimerEntry = std::pair<Timestamp, TimerId>;
  using TimerEntrySet = std::set<TimerEntry>;
  // using ActiveTimer = std::pair<std::unique_ptr<Timer>, int64_t>;
  // using ActiveTimerSet = std::set<ActiveTimer>;

  // 仅在IO线程调用
  void addTimerInLoop(std::unique_ptr<Timer>&& timer);
  void cancelInLoop(TimerId timerId);

  // timerfd_可读时调用
  void handleRead();

  std::vector<TimerEntry> getExpired(Timestamp now);
  // // 重启非一次性定时器
  // void reset(const std::vector<TimerEntry>& expired, Timestamp now);

  bool insert(std::unique_ptr<Timer> timer);

  EventLoop* loop_;
  const int timerfd_;
  Channel timerfdChannel_;
  TimerMap timers_;  // 按Id排列
  TimerEntrySet timerEntries_;

  // // cnacel用
  // ActiveTimerSet activeTimers_;
  // std::atomic<bool> callingExpiredTimers_;
  // ActiveTimerSet cancelingTimers_;
};

}  // namespace net
}  // namespace tmuduo

#endif  // TMUDUO_NET_TIMERQUEUE_Hturn
