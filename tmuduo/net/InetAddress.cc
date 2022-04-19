#include "tmuduo/net/InetAddress.h"

#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <sys/socket.h>

#include "tmuduo/base/Logging.h"
#include "tmuduo/net/Endian.h"
#include "tmuduo/net/SockOps.h"

namespace  
{
using SA = struct sockaddr; 
using SA4 = struct sockaddr_in;
using SA6 = struct sockaddr_in6;
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK; 
#pragma GCC diagnostic error "-Wold-style-cast"

using namespace tmuduo::net;

InetAddress::InetAddress(uint16_t port, bool loopbackonly, bool ipv6)
{
  if (ipv6)
  {
    memZero(&addr6_, sizeof(addr_));
    addr_.sin_family = AF_INET6;
    in6_addr ip = loopbackonly ? in6addr_loopback : in6addr_any;
    addr6_.sin6_addr = ip;
    addr6_.sin6_port = sockets::hostToNetwork16(port);
  }
  else  
  {
    memZero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopbackonly ? kInaddrLoopback : kInaddrAny;
    addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
    addr_.sin_port = sockets::hostToNetwork16(port);
  }
}

InetAddress::InetAddress(const std::string_view& ip, uint16_t port, bool ipv6)
{
  if (ipv6)
  {
    memZero(&addr6_, sizeof(addr6_));
    sockets::fromIpPort(ip.data(), port, &addr6_);
  }
  else  
  {
    memZero(&addr_, sizeof(addr_));
    sockets::fromIpPort(ip.data(), port, &addr_);
  }
}

const struct sockaddr* InetAddress::getSockAddr() const
{ return sockets::cast_to_sockaddr(&addr6_); }

std::string InetAddress::toIp() const
{
  char buf[64] = "";
  sockets::toIp(buf, sizeof(buf), getSockAddr());
  return buf;
}

std::string InetAddress::toIpPort() const
{
  char buf[64] = "";
  sockets::toIpPort(buf, sizeof(buf), getSockAddr());
  return buf;
}

uint16_t InetAddress::getPort() const
{
  return sockets::networkToHost16(portNetEndian());
}

static __thread char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(std::string_view hostname, InetAddress* out)
{
  assert(out != nullptr);
  struct hostent hent;
  struct hostent* pHent = nullptr;
  int hErrno = 0;
  memZero(&hent, sizeof(hent));

  int ret = gethostbyname_r(hostname.data(), 
                            &hent, t_resolveBuffer,
                            sizeof(t_resolveBuffer),
                            &pHent, &hErrno);
  if (ret == 0 && pHent != nullptr)
  {
    assert(pHent->h_addrtype == AF_INET && pHent->h_length == sizeof(uint32_t));
    out->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(pHent->h_addr);
    return true;
  }
  else  
  {
    if (ret)
    {
      LOG_SYSERR << "InetAddress::resolve";
    }
    return true;
  }
}
