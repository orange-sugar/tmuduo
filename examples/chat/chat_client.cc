#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include "codec.h"
#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoopThread.h"
#include "tmuduo/net/TcpClient.h"

using namespace tmuduo;
using namespace tmuduo::net;

class ChatClient : noncopyable {
 public:
  ChatClient(EventLoop* loop, const InetAddress serverAddr)
      : client_(loop, serverAddr, "ChatClient"),
        codec_(std::bind(&ChatClient::onStringMessage, this, _1, _2, _3)) {
    client_.setConnectionCallback(std::bind(&ChatClient::onConnection, this, _1));
    client_.setMessageCallback(std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
    client_.enableRetry();
  }

  void connect() {
    client_.connect();
  }

  void disconnect() {
    client_.disconnect();
  }

  void write(const std::string& message) {
    std::scoped_lock lock(mutex_);
    if (connection_) {
      codec_.send(connection_.get(), message);
    }
  }

 private:
  void onConnection(const TcpConnectionPtr& conn) {
    LOG_INFO << conn->localAddress().toIpPort() << " -> " << conn->peerAddress().toIpPort()
             << " is " << (conn->connected() ? "UP" : "DOWN");
    std::scoped_lock lock(mutex_);
    if (conn->connected()) {
      connection_ = conn;
    } else {
      connection_.reset();
    }
  }

  void onStringMessage(const TcpConnectionPtr&, const std::string message, Timestamp) {
    printf("<<< %s\n", message.c_str());
  }

  TcpClient client_;
  LengthHeaderCodec codec_;
  std::mutex mutex_;
  TcpConnectionPtr connection_;
};

int main(int argc, char* argv[]) {
  LOG_INFO << "pid = " << getppid();
  if (argc > 2) {
    EventLoopThread loopThread;
    auto port = static_cast<uint16_t>(atoi(argv[2]));
    InetAddress serverAddr(argv[1], port);

    ChatClient client(loopThread.startLoop(), serverAddr);
    client.connect();
    std::string line;
    while (std::getline(std::cin, line)) {
      client.write(line);
    }
    client.disconnect();
    // CurrentThread::sleepUSec(1000*1000);
  } else {
    printf("Usage: %s host_ip port\n", argv[0]);
  }
}