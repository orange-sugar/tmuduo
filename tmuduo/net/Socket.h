#ifndef TMUDUO_NET_SOCKET_H
#define TMUDUO_NET_SOCKET_H

#include "tmuduo/base/noncopyable.h"

namespace tmuduo {
namespace net {

class InetAddress;

class Socket : noncopyable {
 public:
  explicit Socket(int sockfd) : sockfd_(sockfd) {
  }

  ~Socket();

  int fd() const {
    return sockfd_;
  }

  void bindAddress(const InetAddress& localaddr);
  void listen();
  int accept(InetAddress* peerAddr);
  void shutdownWrite();

  void setTcpNoDelay(bool on);
  void setReuseAddr(bool on);
  void setReusePort(bool on);
  void setKeepAlive(bool on);

 private:
  const int sockfd_;
};

}  //  namespace net
}  //  namespace tmuduo

#endif  // TMUDUO_NET_SOCKET_H