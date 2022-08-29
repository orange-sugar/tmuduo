#ifndef TMUDUO_NET_POLLER_POLLPOLLER_H
#define TMUDUO_NET_POLLER_POLLPOLLER_H

#include <map>
#include <vector>
#include "tmuduo/net/Poller.h"

struct pollfd;

namespace tmuduo {
namespace net {

class PollPoller : public Poller {
 public:
  PollPoller(EventLoop* loop);
  virtual ~PollPoller();

  virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
  virtual void updateChannel(Channel* channel);
  virtual void removeChannel(Channel* channel);

 private:
  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

  using PollFdList = std::vector<struct pollfd>;
  using ChannelMap = std::map<int, Channel*>;  // 文件描述符作为key

  PollFdList pollfds_;
  ChannelMap channels_;
};

}  // namespace net
}  // namespace tmuduo

#endif  // TMUDUO_NET_POLLER_POLLPOLLER_H