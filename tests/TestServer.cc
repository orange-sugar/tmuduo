#include "tmuduo/net/TcpServer.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/InetAddress.h" 
#include "tmuduo/base/Logging.h"

#include <stdio.h>

using namespace tmuduo::net;

class TestServer
{
  public:
    TestServer(EventLoop* loop, const InetAddress& listenAddr)
      : loop_(loop),
        server_(loop, listenAddr, "TestServer", TcpServer::Option::kNoReusePort)
    {
      server_.setConnectionCallback(
        std::bind(&TestServer::onConnection, this, _1)
      );
      server_.setMessageCallback(
        std::bind(&TestServer::onMessage, this, _1, _2, _3)
      );
    }
    void start()
    {
      server_.start();
    }
  
  private:

    void onConnection(const TcpConnectionPtr& conn)
    {
      if (conn->connected())
      {
        printf("onConnection(): new Connection [%s] from %s\n",
               conn->name().c_str(),
               conn->peerAddress().toIpPort().c_str());
      }
      else  
      {
        printf("onConnection(): connection [%s] is down\n",
               conn->name().c_str());
      }
    }

    void onMessage(const TcpConnectionPtr& conn,
                   const char* buf,
                   ssize_t len)
    {
      printf("onMessage(): received %zd bytes from connection [%s]\n",
             len, conn->name().c_str());
    }

    EventLoop* loop_;
    TcpServer server_;
};

int main()
{
  Logger::setLogLevel(Logger::DEBUG);
  EventLoop loop;
  InetAddress listenAddr(6912);
  TestServer server(&loop, listenAddr);
  server.start();
  loop.loop();
}