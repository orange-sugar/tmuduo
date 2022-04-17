#ifndef TMUDUO_NET_EVENTLOOP_H
#define TMUDUO_NET_EVENTLOOP_H

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "tmuduo/base/noncopyable.h"
#include "tmuduo/base/CurrentThread.h"

#include "tmuduo/net/Callbacks.h"
#include "tmuduo/net/TimerId.h"

namespace tmuduo
{
namespace detail {

// https://microeducate.tech/passing-a-non-copyable-closure-object-to-stdfunction-parameter-duplicate/
template< class F >
auto make_copyable_function( F&& f ) {
  using dF = std::decay_t<F>;
  auto spf = std::make_shared<dF>( std::forward<F>(f) );
  return [spf](auto&&... args)->decltype(auto) {
    return (*spf)( decltype(args)(args)... );
  };
}

// https://kenkyu-note.hatenablog.com/entry/2019/10/06/194822
template <class Func>
struct CopyableFunction
{
    CopyableFunction(Func f)
      : func(std::move(f))
    {}
    CopyableFunction(const CopyableFunction&)
      : func(ThrowException())
    {}
    CopyableFunction(CopyableFunction&&) = default;
    Func ThrowException() { throw std::exception(); }

    template <class ...Args>
    decltype(auto) operator()(Args&& ...args) const { return func(std::forward<Args>(args)...); }
    template <class ...Args>
    decltype(auto) operator()(Args&& ...args) { return func(std::forward<Args>(args)...); }
    Func func;
};
template <class Func>
CopyableFunction<Func> MakeCopyableFunction(Func func) { return CopyableFunction<Func>(std::move(func)); }

} // namespace detail
namespace net {

class Channel;
class Poller;
class TimerQueue;

class EventLoop : noncopyable
{
  public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void assertInLoopThread()
    {
      if (!isInLoopThread())
      {
        abortNotInLoopThread();
      }
    }

    void runInLoop(Functor cb);

    void queueInLoop(Functor cb);

    // 定时任务
    TimerId runAt(Timestamp time, TimerCallback cb);
    TimerId runAfter(double delay, TimerCallback cb);
    TimerId runEvery(double interval, TimerCallback cb);
    void cancel(TimerId timerId);

    void wakeup();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
    static EventLoop* getEventLoopOfCurrentThread();

  private:

    using ChannelList = std::vector<Channel*>;
    void abortNotInLoopThread();

    void readWakeupFd(); // wake up
    void doPendingFunctors();

    //debug
    void printActiveChannels() const;

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> eventHandling_;
    std::atomic<bool> callingPendingFunctors_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;
    int wakeupFd_; // IO线程外唤醒用
    ChannelList activeChannels_;
    // remove用
    Channel* currentActiveChannel_;
    // IO线程外唤醒用
    std::unique_ptr<Channel> wakeupChannel_;
    std::mutex mutex_;
    std::vector<Functor> pendingFunctors_;
};

} //  namespace tmuduo::net
} //  namespace tmuduo



#endif	// TMUDUO_NET_EVENTLOOP_H