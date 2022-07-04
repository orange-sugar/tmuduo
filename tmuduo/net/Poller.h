#ifndef TMUDUO_NET_POLLER_H
#define TMUDUO_NET_POLLER_H

#include <vector>

#include "tmuduo/base/Timestamp.h"
#include "tmuduo/net/EventLoop.h"

namespace tmuduo 
{
namespace net 
{

class Channel;

class Poller : noncopyable
{
  public:
    using ChannelList = std::vector<Channel*>;
    
    Poller(EventLoop* loop);
    virtual ~Poller();

    
    /// 轮询IO事件
    /// 需要在IO线程调用
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

    /// 修改欲关注的IO事件 
    /// 需要在IO线程调用
    virtual void updateChannel(Channel* channel) = 0;

    /// 删除channel 
    /// 需要在IO线程调用
    virtual void removeChannel(Channel* channel) = 0;

    static Poller* newDefaultPoller(EventLoop* loop);

    void assertInLoopThread()
    {
      ownerLoop_->assertInLoopThread();
    }

  private:
    EventLoop* ownerLoop_;
};

} //  namespace tmuduo::net
} //  namespace tmuduo 


#endif	// TMUDUO_NET_POLLER_H