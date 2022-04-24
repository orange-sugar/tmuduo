#ifndef TMUDUO_NET_TCPCONNECTION_H
#define TMUDUO_NET_TCPCONNECTION_H

#include <any>
#include <atomic>
#include <memory>
#include <mutex>

#include "tmuduo/base/noncopyable.h"
#include "tmuduo/base/Types.h"
#include "tmuduo/net/Buffer.h"
#include "tmuduo/net/Callbacks.h"
#include "tmuduo/net/InetAddress.h"

namespace tmuduo
{
namespace net
{

class Channel;
class EventLoop;
class Socket;

// 封装Tcp连接，用于server或client
// enable_share_from_this可以生成共享该对象的share_ptr
class TcpConnection : noncopyable, 
                      public std::enable_shared_from_this<TcpConnection>
{
  public:
    // 用处于连接状态的sockfd构造TcpConnection对象
    TcpConnection(EventLoop* loop,
                  std::string name,
                  int sockfd,
                  InetAddress localAddr,
                  InetAddress peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const std::string name() const { return name_; }
    const InetAddress& localAddress() const { return localAddress_; }
    const InetAddress& peerAddress() const { return peerAddress_; }
    bool connected() const { return state_ == StateE::kConnected; }

    void send(const void* message, size_t len);
    void send(const std::string_view& message);
    void send(Buffer&& message);

    void shutdown();

    void setTcpNoDelay(bool on);

    void setContext(const std::any& context)
    { context_ = context; }

    std::any* getMutableContext()
    { return &context_; }

    // 设置各事件回调
    void setConnectionCallbcak(ConnectionCallback cb)
    { connectionCallback_ = std::move(cb); }
    void setMessageCallback(MessageCallback cb)
    { messageCallback_ = std::move(cb); }
    void setWriteCompleteCallback(WriteCompleteCallback cb)
    { writeCompleteCallback_ = std::move(cb); }
    void setHighWaterMarkCallback(HighWaterMarkCallback cb)
    { highWaterMarkCallback_ = std::move(cb); }

    void setCloseCallback(CloseCallback cb)
    { closeCallback_ = std::move(cb); }

    void connectionEstablished();
    void connectionDestroyed();

  private:
    enum class StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const std::string_view& message);
    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();

    void setState(StateE s) { state_.store(s); }

    EventLoop* loop_;
    std::string name_;
    std::atomic<StateE> state_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    InetAddress localAddress_;
    InetAddress peerAddress_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    // 数据发送完毕（全部拷贝至内核区）/输出缓冲区被清空时回调
    WriteCompleteCallback writeCompleteCallback_;
    // 高水位回调
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;
    size_t highWaterMark_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
    std::any context_;
};


} // namespace net
} // namespace tmuduo

#endif	// TMUDUO_NET_TCPCONNECTION_H