#ifndef TMUDUO_NET_TCPSERVER_H
#define TMUDUO_NET_TCPSERVER_H

#include <atomic>
#include <map>
#include <memory>

#include "tmuduo/base/noncopyable.h"
#include "tmuduo/net/TcpConnection.h"

namespace tmuduo
{
namespace net
{

class Acceptor;
class EventLoop;

class TcpServer : noncopyable
{
  public:
    enum class Option
    {
      kNoReusePort,
      kReusePort,
    };

    TcpServer(EventLoop* loop,
              InetAddress listenAddr,
              std::string nameArg,
              Option option);
    ~TcpServer();

    const std::string& hostport() const { return hostport_; }
    const std::string& name() const { return name_; }

    void start();

    void setConnectionCallback(ConnectionCallback cb)
    { connectionCallback_ = std::move(cb); }

    void setMessageCallback(MessageCallback cb)
    { messageCallback_ = std::move(cb); }

  private:
    using ConnectionMap = std::map<std::string,
                                   TcpConnectionPtr>;
    void newConnection(int sockfd, InetAddress peerAddr);

    EventLoop* loop_;
    std::string hostport_;
    std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    std::atomic<bool> started_;

    int nextConnId_;
    ConnectionMap connections_;
};

} // namespace net
} // namespace tmuduo

#endif	// TMUDUO_NET_TCPSERVER_H