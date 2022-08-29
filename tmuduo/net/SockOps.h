#ifndef TMUDUO_NET_SOCKOPS_H
#define TMUDUO_NET_SOCKOPS_H

#include <arpa/inet.h>
#include "tmuduo/base/Types.h"

namespace tmuduo {
namespace net {

namespace sockets {

const struct sockaddr* cast_to_sockaddr(const struct sockaddr_in* addr);
struct sockaddr* cast_to_sockaddr(struct sockaddr_in6* addr);
const struct sockaddr* cast_to_sockaddr(const struct sockaddr_in6* addr);
const struct sockaddr_in* cast_to_sockaddr_in(const struct sockaddr* addr);
const struct sockaddr_in6* cast_to_sockaddr_in6(const struct sockaddr* addr);

void toIp(char* buf, size_t size, const struct sockaddr* addr);
void toIpPort(char* buf, size_t size, const struct sockaddr* addr);
void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);
void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr);

int createNonblockingOrDie(sa_family_t family);
void bindOrDie(int sockfd, const struct sockaddr* addr);
void listenOrDie(int sockfd);
int accept(int sockfd, struct sockaddr_in6* addr);
int connect(int sockfd, const struct sockaddr* addr);
ssize_t read(int sockfd, void* buf, size_t count);
ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);
ssize_t write(int sockfd, const void* buf, size_t count);
void close(int sockfd);
void shutdownWrite(int sockfd);

int getSocketError(int sockfd);

struct sockaddr_in6 getLocalAddr(int sockfd);
struct sockaddr_in6 getPeerAddr(int sockfd);
bool isSelfConnect(int sockfd);
}  // namespace sockets

}  // namespace net
}  // namespace tmuduo

#endif  // TMUDUO_NET_SOCKOPS_H