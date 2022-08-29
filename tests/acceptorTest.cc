#include "tmuduo/net/Acceptor.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/InetAddress.h"
#include "tmuduo/net/SockOps.h"

using namespace tmuduo;
using namespace tmuduo::net;

void newConnection(int sockfd, const InetAddress& peerAddr) {
  printf("newConnection(): accepted a new connection from %s\n", peerAddr.toIpPort().c_str());
  ::write(sockfd, "How are you?\n", 13);
  sockets::close(sockfd);
}

int main() {
  EventLoop loop;
  InetAddress listenAddr(9981);
  Acceptor acceptor(&loop, listenAddr, false);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();
  loop.loop();
}