#include "tmuduo/net/TcpServer.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/InetAddress.h"

using namespace tmuduo::net;

void onConnection(const TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().data(),
           conn->peerAddress().toIpPort().c_str());
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->name().data());
  }
}

void onMessage(const TcpConnectionPtr& conn,
               const char* data,
               ssize_t len)
{
  printf("onMessage(): received %zd bytes from connection [%s]\n",
         len, conn->name().data());
}

int main()
{
  EventLoop loop;
  InetAddress listenAddr(6912);
  TcpServer server(&loop, listenAddr, "orange", TcpServer::Option::kNoReusePort);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();
  loop.loop();
}