#ifndef TMUDUO_NET_INETADDRESS_H
#define TMUDUO_NET_INETADDRESS_H

#include <netinet/in.h>

#include <string_view>

#include "tmuduo/base/copyable.h"
// #include "tmuduo/net/SockOps.h"
// #include "tmuduo/base/StringPiece.h"

namespace tmuduo
{
namespace net
{

class InetAddress : public copyable
{
  public:
    explicit InetAddress(uint16_t port = 0, bool loopbackonly = false,  bool ipv6 = false);

    InetAddress(const std::string_view& ip, uint16_t port, bool ipv6 = false);

    explicit InetAddress(const sockaddr_in& addr)
      : addr_(addr)
    { }
    explicit InetAddress(const sockaddr_in6& addr)
      : addr6_(addr)
    { }


    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t getPort() const;

    const struct sockaddr* getSockAddr() const;
    void setSockAddrInet6(const struct sockaddr_in6& addr) { addr6_ = addr; }

    uint32_t ipNetEndian() const { return addr_.sin_addr.s_addr; }
    uint16_t portNetEndian() const { return addr_.sin_port; }
    
    bool resolve(std::string_view hostname, InetAddress* out);

  private:
    // 管理一个socket地址(ipv4/ipv6)
    union {
      struct sockaddr_in addr_;
      struct sockaddr_in6 addr6_;
    };
    
};

} //  namespace net
} //  namespace tmuduo

#endif	// TMUDUO_NET_INETADDRESS_H