#include "tmuduo/net/TcpServer.h"

#include "tmuduo/base/Logging.h"
#include "tmuduo/net/Acceptor.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/SockOps.h"

using namespace tmuduo::net;

TcpServer::TcpServer(EventLoop* loop,
                    InetAddress listenAddr,
                    std::string nameArg,
                    Option option)
  : loop_(loop),
    hostport_(listenAddr.toIpPort()),
    name_(nameArg),
    acceptor_(new Acceptor(loop, listenAddr, option == Option::kReusePort)),
    started_(false),
    nextConnId_(1)
{
  acceptor_->setNewConnectionCallback(
    std::bind(&TcpServer::newConnection, this, _1, _2)
  );
}

TcpServer::~TcpServer()
{
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] dtor";

  for (auto & item : connections_)
  {
    auto connPtr = item.second;
    item.second.reset();
    connPtr->getLoop()->runInLoop(
      std::bind(&TcpConnection::connectionDestroyed, connPtr)
    );
  }
}

void TcpServer::start()
{
  if (!started_)
  {
    started_.store(true);
  }
  if (!acceptor_->listenning())
  {
    loop_->runInLoop(
      std::bind(&Acceptor::listen, acceptor_.get())
    );
  }
}

void TcpServer::newConnection(int sockfd, InetAddress peerAddr)
{
  loop_->assertInLoopThread();
  char buf[32];
  snprintf(buf, sizeof(buf), ":%s#%d", hostport_.c_str(), nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf;

  LOG_INFO << "TcpServer::newConnection [" << name_
           << "] - new connection [" << connName
           << "] from " << peerAddr.toIpPort();
  
  InetAddress localAddr(sockets::getLocalAddr(sockfd));
  auto conn = std::make_shared<TcpConnection>(loop_, 
                                              connName,
                                              sockfd,
                                              localAddr,
                                              peerAddr);
LOG_DEBUG << "[1] usecount=" << conn.use_count();
  connections_[connName] = conn;
LOG_DEBUG << "[2] usecount=" << conn.use_count();
  conn->setConnectionCallbcak(connectionCallback_);
  conn->setMessageCallback(messageCallback_);

  conn->setCloseCallback(
    std::bind(&TcpServer::removeConnection, this, _1)
  );

  conn->connectionEstablished();
}


void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
  loop_->assertInLoopThread();
  LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
           << "] - connection " << conn->name();
  size_t n = connections_.erase(conn->name());
  (void)n; assert(n == 1);
  loop_->runInLoop(
    std::bind(&TcpConnection::connectionDestroyed, conn)
  );
}