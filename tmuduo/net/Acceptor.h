#ifndef TMUDUO_NET_ACCEPTOR_H
#define TMUDUO_NET_ACCEPTOR_H

#include <functional>
#include "tmuduo/base/noncopyable.h"
#include "tmuduo/net/Channel.h"
#include "tmuduo/net/Socket.h"

namespace tmuduo {
namespace net {

class EventLoop;
class InetAddress;

class Acceptor : noncopyable {
 public:
  using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;
  Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
  ~Acceptor();

  void setNewConnectionCallback(NewConnectionCallback cb) {
    newConnectionCallback_ = std::move(cb);
  }

  void listen();
  bool listenning() const {
    return listenning_;
  }

 private:
  void handleRead();

  EventLoop* loop_;
  Socket acceptSocket_;
  Channel acceptchannel_;
  NewConnectionCallback newConnectionCallback_;
  bool listenning_;
  int idleFd_;
};

}  // namespace net
}  // namespace tmuduo

#endif  // TMUDUO_NET_ACCEPTOR_H