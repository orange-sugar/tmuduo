#include "tmuduo/net/TcpClient.h"

#include "tmuduo/net/Connector.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/SockOps.h"

#include <stdio.h>

using namespace tmuduo;
using namespace tmuduo::net;

namespace tmuduo
{
namespace net
{

namespace detail
{
void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn)
{
  loop->queueInLoop(std::bind(&TcpConnection::connectionDestroyed, conn));
}

void removeConnector(const ConnectorPtr& connector)
{
  //connector->
}
}

} //  namespace net
} //  namespace tmuduo

TcpClient::TcpClient(EventLoop* loop,
                     const InetAddress& serverAddr,
                     const std::string_view& nameArg)
  : loop_(loop),
    connector_(new Connector(loop, serverAddr)),
    name_(nameArg),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback),
    retry_(false),
    connect_(true),
    nextConnId_(1)
{
  connector_->setNewConnectionCallback(
    std::bind(&TcpClient::newConnection, this, _1));
  LOG_INFO << "TcpClient::TcpClient[" << name_
           << "] - connector " << connector_.get();
}

TcpClient::~TcpClient()
{
  LOG_INFO << "TcpClient::~TcpClient[" << name_
           << "] - connector " << connector_.get();
  TcpConnectionPtr conn;
  {
    std::scoped_lock lock(mutex_);
    conn = connection_;
  }
  if (conn)
  {
    assert(loop_ == conn->getLoop());
    // FIXME: not 100% safe, if we are in different thread
    CloseCallback cb = std::bind(&detail::removeConnection, loop_, _1);
    loop_->runInLoop(
        std::bind(&TcpConnection::setCloseCallback, conn, cb));
  }
  else
  {
    connector_->stop();
    // FIXME: HACK
    loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
  }
}

void TcpClient::connect()
{
  LOG_INFO << "TcpClient::connect[" << name_ << "] - connecting to "
           << connector_->serverAddress().toIpPort();
  connect_ = true;
  connector_->start();
}

void TcpClient::disconnect()
{
  connect_ = false;
  {
    std::scoped_lock lock(mutex_);
    if (connection_)
    {
      connection_->shutdown();
    }
  }
}

void TcpClient::stop()
{
  connect_ = false;
  connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
  loop_->assertInLoopThread();
  InetAddress peerAddr(sockets::getPeerAddr(sockfd));
  char buf[32];
  snprintf(buf, sizeof(buf), ":%s#%d", peerAddr.toIpPort().c_str(), nextConnId_);
  ++nextConnId_;
  std::string connName  = name_ + buf;

  InetAddress localAddr(sockets::getLocalAddr(sockfd));

  TcpConnectionPtr conn(new TcpConnection(loop_,
                                          connName,
                                          sockfd,
                                          localAddr,
                                          peerAddr));
  conn->setConnectionCallbcak(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  conn->setCloseCallback(
      std::bind(&TcpClient::removeConnection, this, _1));
  {
    std::scoped_lock lock(mutex_);
    connection_ = conn;
  }
  conn->connectionEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
  loop_->assertInLoopThread();
  assert(loop_ == conn->getLoop());

  {
    std::scoped_lock lock(mutex_);
    assert(connection_ == conn);
    connection_.reset();
  }

  loop_->queueInLoop(std::bind(&TcpConnection::connectionDestroyed, conn));
  if (retry_ && connect_)
  {
    LOG_INFO << "TcpClient::connect[" << name_ << "] - Reconnecting to "
             << connector_->serverAddress().toIpPort();
    // 连接建立成功之后被断开的重连
    connector_->restart();
  }
}