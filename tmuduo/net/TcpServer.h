#ifndef TMUDUO_NET_TCPSERVER_H
#define TMUDUO_NET_TCPSERVER_H

#include <atomic>
#include <map>
#include <memory>
#include "tmuduo/base/noncopyable.h"
#include "tmuduo/net/TcpConnection.h"

namespace tmuduo {
namespace net {

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : noncopyable {
 public:
  using ThreadInitCallback = std::function<void(EventLoop*)>;
  enum class Option {
    kNoReusePort,
    kReusePort,
  };

  TcpServer(EventLoop* loop, InetAddress listenAddr, std::string nameArg,
            Option option = Option::kNoReusePort);
  ~TcpServer();

  const std::string& hostport() const {
    return hostport_;
  }
  const std::string& name() const {
    return name_;
  }
  EventLoop* getLoop() const {
    return loop_;
  }

  void setThreadNum(int numThreads);
  void setThreadInitCallback(ThreadInitCallback cb) {
    threadInitCallback_ = std::move(cb);
  }

  void start();

  void setConnectionCallback(ConnectionCallback cb) {
    connectionCallback_ = std::move(cb);
  }

  void setMessageCallback(MessageCallback cb) {
    messageCallback_ = std::move(cb);
  }

  void setWriteCompleteCallback(WriteCompleteCallback cb) {
    writeCompleteCallback_ = std::move(cb);
  }

 private:
  using ConnectionMap = std::map<std::string, TcpConnectionPtr>;
  void newConnection(int sockfd, InetAddress peerAddr);

  void removeConnection(const TcpConnectionPtr& conn);

  void removeConnectionInLoop(const TcpConnectionPtr& conn);

  EventLoop* loop_;
  std::string hostport_;
  std::string name_;
  std::unique_ptr<Acceptor> acceptor_;
  std::unique_ptr<EventLoopThreadPool> threadPool_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  ThreadInitCallback threadInitCallback_;
  std::atomic<bool> started_;

  int nextConnId_;
  ConnectionMap connections_;
};

}  // namespace net
}  // namespace tmuduo

#endif  // TMUDUO_NET_TCPSERVER_H