#ifndef TMUDUO_NET_POLLER_EPOLLPOLLER_H
#define TMUDUO_NET_POLLER_EPOLLPOLLER_H

#include "tmuduo/net/Poller.h"

#include <map>
#include <vector>

struct epoll_event;

namespace tmuduo
{
namespace net
{

class EPollPoller : public Poller  
{
  public:
    EPollPoller(EventLoop* loop);
    ~EPollPoller();

    virtual  Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
    virtual void updateChannel(Channel* channel) override;
    virtual void removeChannel(Channel* channel) override;

  private:
    static const int kInitEventListSize = 16;
    void fillActiveChannels(int numEvents, ChannelList* activeChannels);
    void update(int operation, Channel* channel);
    
    using EventList = std::vector<struct epoll_event>;
    using ChannelMap = std::map<int, Channel*>;

    int epollfd_;
    EventList events_;
    ChannelMap channels_;
};

}  // namespace tmuduo
}  // namespace net

#endif	// TMUDUO_NET_POLLER_EPOLLPOLLER_H