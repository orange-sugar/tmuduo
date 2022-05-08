#include "tmuduo/net/Socket.h"

#include <netinet/tcp.h>

#include "tmuduo/base/Logging.h"
#include "tmuduo/net/InetAddress.h"
#include "tmuduo/net/SockOps.h"

using namespace tmuduo;
using namespace tmuduo::net;

Socket::~Socket()
{
  sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& localAddr)
{
  sockets::bindOrDie(sockfd_, localAddr.getSockAddr());
}

void Socket::listen()
{
  sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peerAddr)
{
  struct sockaddr_in6 addr;
  memZero(&addr, sizeof(addr));
  int connfd = sockets::accept(sockfd_, &addr);
  if (connfd >= 0)
  {
    peerAddr->setSockAddrInet6(addr);
  }
  return connfd;
}

void Socket::shutdownWrite()
{
  sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on)
{
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof(optval)));
  if (ret < 0 && on)
  {
    LOG_SYSERR << "TCP_NODELAY failed";
  }
}

void Socket::setReuseAddr(bool on)
{
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof(optval)));
  if (ret < 0 && on)
  {
    LOG_SYSERR << "SO_REUSEPORT failed";
  }
}

void Socket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof(optval)));
  if (ret < 0 && on)
  {
    LOG_SYSERR << "SO_REUSEPORT failed.";
  }
#else
  if (on)
  {
    LOG_SYSERR << "SO_REUSEPORT is not supported.";
  }
#endif
}

void Socket::setKeepAlive(bool on)
{
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
  if (ret < 0 && on)
  {
    LOG_SYSERR << "SO_KEEPALIVE fialed";
  }
}