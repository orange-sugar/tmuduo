#include <stdio.h>
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/InetAddress.h"
#include "tmuduo/net/TcpServer.h"

using namespace tmuduo;
using namespace tmuduo::net;

class TestServer {
 public:
  TestServer(EventLoop* loop, const InetAddress& listenAddr)
      : loop_(loop), server_(loop, listenAddr, "TestServer", TcpServer::Option::kReusePort) {
    server_.setConnectionCallback(std::bind(&TestServer::onConnection, this, _1));
    server_.setMessageCallback(std::bind(&TestServer::onMessage, this, _1, _2, _3));
    server_.setWriteCompleteCallback(std::bind(&TestServer::onWriteComplete, this, _1));

    std::string line;
    for (int i = 33; i < 127; ++i) {
      line.push_back(char(i));
    }
    line += line;

    for (size_t i = 0; i < 127 - 33; ++i) {
      message_ += line.substr(i, 72) + '\n';
    }
  }

  void start() {
    server_.start();
  }

 private:
  void onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
      printf("onConnection(): new connection [%s] from %s\n", conn->name().c_str(),
             conn->peerAddress().toIpPort().c_str());

      conn->setTcpNoDelay(true);
      conn->send(message_);
    } else {
      printf("onConnection(): connection [%s] is down\n", conn->name().c_str());
    }
  }

  void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime) {
    std::string msg(buf->retrieveAllAsString());
    printf("onMessage(): received %zd bytes from connection [%s] at %s\n", msg.size(),
           conn->name().c_str(), receiveTime.toFormattedString().c_str());

    conn->send(msg);
  }

  void onWriteComplete(const TcpConnectionPtr& conn) {
    conn->send(message_);
  }

  EventLoop* loop_;
  TcpServer server_;

  std::string message_;
};

int main() {
  printf("main(): pid = %d\n", getpid());

  InetAddress listenAddr(6912);
  EventLoop loop;

  TestServer server(&loop, listenAddr);
  server.start();

  loop.loop();
}