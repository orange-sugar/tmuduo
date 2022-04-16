#include "tmuduo/net/poller/PollPoller.h"

#include <poll.h>

#include "tmuduo/net/Channel.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/base/Logging.h"

namespace tmuduo
{
namespace net
{

PollPoller::PollPoller(EventLoop* loop)
  : Poller(loop)
{
}

PollPoller::~PollPoller()
{
}

Timestamp PollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
  int numEvents = ::poll(pollfds_.data(), pollfds_.size(), timeoutMs);
  Timestamp now = Timestamp::now();
  if (numEvents > 0)
  {
    LOG_TRACE << numEvents << " events happened";
    fillActiveChannels(numEvents, activeChannels);
  }
  else if (numEvents == 0)  
  {
    LOG_TRACE << " nothing happened";
  }
  else  
  {
    LOG_SYSERR << "PollPoller::poll()";
  }  
  return now;
}

void PollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
  for (const auto pfd: pollfds_)
  {
    if (numEvents > 0)
    {
      --numEvents;
      ChannelMap::const_iterator it = channels_.find(pfd.fd);
      assert(it != channels_.end());
      Channel* channel = it->second;
      assert(channel->fd() == pfd.fd);
      channel->setRevents(pfd.revents);
      activeChannels->push_back(channel);
    }
  }
}

void PollPoller::updateChannel(Channel* channel)
{
  Poller::assertInLoopThread();
  LOG_TRACE << "fd = " << channel->fd() << ", events = " << channel->events();
  if (channel->index() < 0) // 新加入channel
  {
    assert(channels_.find(channel->fd()) == channels_.end());
    struct pollfd pfd;
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    pollfds_.push_back(pfd);
    int idx = static_cast<int>(pollfds_.size()) - 1;
    channel->setIndex(idx);
    channels_[pfd.fd] = channel;
  }
  else // 更新已存在channel  
  {
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
    struct pollfd& pfd = pollfds_[static_cast<size_t>(idx)];
    assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    // 将一个channel暂时更改为不关注状态，但并未从Poller中移除
    if (channel->isNoneEvent())
    {
      // 方便removeChannel
      pfd.fd = -channel->fd()-1;
    }
  }
}

void PollPoller::removeChannel(Channel* channel)
{
  Poller::assertInLoopThread();
  LOG_TRACE << "fd = " << channel->fd();
  assert(channels_.find(channel->fd()) != channels_.end());
  assert(channels_[channel->fd()] == channel);
  assert(channel->isNoneEvent());
  int idx = channel->index();
  assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
  const struct pollfd& pfd = pollfds_[static_cast<size_t>(idx)];
  assert(pfd.fd == -channel->fd()-1 && channel->events() == pfd.events); (void)pfd;
  size_t n = channels_.erase(channel->fd());
  assert(n == 1); (void)n; // (void)n; 消除变量未用警告
  if (implicit_cast<size_t>(idx) == pollfds_.size() - 1)
  {
    pollfds_.pop_back();
  }
  else  
  {
    int channelAtEnd = pollfds_.back().fd;
    std::iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
    if (channelAtEnd < 0)
    {
      channelAtEnd = -channelAtEnd-1;
    }
    channels_[channelAtEnd]->setIndex(idx);
    pollfds_.pop_back();
  }

}

}  // namespace tmuduo
}  // namespace net