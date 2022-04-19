#include "tmuduo/net/Socket.h"
#include "tmuduo/net/InetAddress.h"
#include "tmuduo/net/SockOps.h"

using namespace tmuduo::net;

int main()
{
  InetAddress localAddr(9981);
  Socket sock(sockets::createNonblockingOrDie(AF_INET));
  sock.setReuseAddr(true);
  sock.bindAddress(localAddr);
  sock.listen();
  InetAddress peerAddr;
  memZero(&peerAddr, sizeof(peerAddr));
  sock.accept(&peerAddr);
  
  // char buf[1024];
  
  // printf("%s\n", inet_ntoa(ss->sin_addr));
}