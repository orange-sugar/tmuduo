#ifndef TIMERID_H
#define TIMERID_H

#include "tmuduo/base/copyable.h"
#include "tmuduo/base/Types.h"

namespace tmuduo
{
namespace net
{

class Timer;

struct TimerId : public copyable
{
  public:

    TimerId(int64_t seq = 0)
      : sequence_(seq)
    {
      // LOG_DEBUG << "ctor[" << this <<"] seq: " << sequence_;
    }

    int64_t sequence() const  
    { return sequence_; }

    friend class TimerQueue;

    friend bool operator==(TimerId lhs, TimerId rhs);
    friend bool operator<(TimerId lhs, TimerId rhs);

    int64_t sequence_;
};

inline bool operator<(TimerId lhs, TimerId rhs)
{
  return lhs.sequence_ < rhs.sequence_;
}

inline bool operator==(TimerId lhs, TimerId rhs)
{
  return lhs.sequence_ == rhs.sequence_;
}

}  // namespace tmuduo
}  // namespace net

namespace std {
template <>
struct hash<typename tmuduo::net::TimerId> {
    size_t operator()(tmuduo::net::TimerId timerId) const
    {
        // return static_cast<size_t>(timerId.sequence_ ^ reinterpret_cast<uintptr_t>(timerId.timer_));
      return static_cast<size_t>(timerId.sequence_);
    }
};
}

#endif //  TIMERID_H