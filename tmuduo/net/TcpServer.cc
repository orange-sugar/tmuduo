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
  connections_[connName] = conn;
  conn->setConnectionCallbcak(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->connectionEstablished();
}