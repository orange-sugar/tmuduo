#ifndef TMUDUO_NET_TCPCLIENT_H
#define TMUDUO_NET_TCPCLIENT_H

#include <memory>
#include <mutex>

#include "tmuduo/base/noncopyable.h"
#include "tmuduo/net/TcpConnection.h"

namespace tmuduo
{
namespace net
{

class Connector;
using ConnectorPtr = std::shared_ptr<Connector>;

class TcpClient : noncopyable
{
  public:
    TcpClient(EventLoop* loop,
              const InetAddress& serverAddr,
              const std::string_view& nameArg);
    ~TcpClient();  // force out-line dtor, for std::unique_ptr members.

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() const
    {
      std::scoped_lock lock(mutex_);
      return connection_;
    }
    
     EventLoop* getLoop() const { return loop_; }
    bool retry() const { return retry_; }
    void enableRetry() { retry_ = true; }

    const std::string& name() const
    { return name_; }

    /// Set connection callback.
    /// Not thread safe.
    void setConnectionCallback(ConnectionCallback cb)
    { connectionCallback_ = std::move(cb); }

    /// Set message callback.
    /// Not thread safe.
    void setMessageCallback(MessageCallback cb)
    { messageCallback_ = std::move(cb); }

    /// Set write complete callback.
    /// Not thread safe.
    void setWriteCompleteCallback(WriteCompleteCallback cb)
    { writeCompleteCallback_ = std::move(cb); }


  private:
    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* loop_;
    ConnectorPtr connector_;	// 用于主动发起连接
    const std::string name_;		// 名称
    ConnectionCallback connectionCallback_;		// 连接建立回调函数
    MessageCallback messageCallback_;				// 消息到来回调函数
    WriteCompleteCallback writeCompleteCallback_;	// 数据发送完毕回调函数
    bool retry_;   
    bool connect_; 
    int nextConnId_;
    mutable std::mutex mutex_;
    TcpConnectionPtr connection_;
};

} //  namespace net
} //  namespace tmuduo

#endif	// TMUDUO_NET_TCPCLIENT_H