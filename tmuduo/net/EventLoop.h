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

    void handleRead(); // wake up
    void doPendingFunctors();

    //debug
    void printActiveChannels() const;

    //std::atomic<bool> 
    bool looping_;
    std::atomic<bool> quit_;
    // std::atomic<bool> 
    bool eventHandling_;
    // std::atomic<bool> 
    bool callingPendingFunctors_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;
    int wakeupFd_; // IO线程外唤醒用
    ChannelList activeChannels_;
    
    // IO线程外唤醒用
    std::unique_ptr<Channel> wakeupChannel_;
    // remove用
    Channel* currentActiveChannel_;

    std::mutex mutex_;
    std::vector<Functor> pendingFunctors_;
};

} //  namespace tmuduo::net
} //  namespace tmuduo



#endif	// TMUDUO_NET_EVENTLOOP_H