#ifndef TMUDUO_NET_POLLER_EPOLLPOLLER_H
#define TMUDUO_NET_POLLER_EPOLLPOLLER_H

#include <map>
#include <vector>
#include "tmuduo/net/Poller.h"

struct epoll_event;

namespace tmuduo {
namespace net {

class EPollPoller : public Poller {
 public:
  EPollPoller(EventLoop* loop);
  ~EPollPoller();

  // 调用epoll接口，更新各个channel的revent
  virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;

  // 添加/修改channel对应的fd
  virtual void updateChannel(Channel* channel) override;

  // 删除channel对应的fd
  virtual void removeChannel(Channel* channel) override;

 private:
  static const int kInitEventListSize = 16;
  void fillActiveChannels(int numEvents, ChannelList* activeChannels);
  const char* operationToString(int op);
  void update(int operation, Channel* channel);

  using EventList = std::vector<struct epoll_event>;
  using ChannelMap = std::map<int, Channel*>;

  int epollfd_;
  EventList events_;
  ChannelMap channels_;
};

}  // namespace net
}  // namespace tmuduo

#endif  // TMUDUO_NET_POLLER_EPOLLPOLLER_H