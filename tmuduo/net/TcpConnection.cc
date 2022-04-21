#include "tmuduo/net/TcpConnection.h"



#include "tmuduo/base/Logging.h"
#include "tmuduo/net/Channel.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/Socket.h"
#include "tmuduo/net/SockOps.h"

using namespace tmuduo::net;

TcpConnection::TcpConnection(EventLoop* loop,
                  std::string name,
                  int sockfd,
                  InetAddress localAddr,
                  InetAddress peerAddr)
  : loop_(loop),
    name_(std::move(name)),
    state_(StateE::kConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddress_(std::move(localAddr)),
    peerAddress_(std::move(peerAddr))
{
  channel_->setReadCallback(
    std::bind(&TcpConnection::handleRead, this, _1)
  );
  LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this
            << " fd=" << sockfd;
  socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at " << this
          << " fd=" << channel_->fd();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
  loop_->assertInLoopThread();
  int savedErrno = 0;
  char buf[65536];
  ssize_t n = ::read(channel_->fd(), buf, sizeof(buf));
  if (n > 0)
  {
    messageCallback_(shared_from_this(), buf, n);
  }
  else if (n == 0)
  {
    handleClose();
  }
  else  
  {
    errno = savedErrno;
    LOG_SYSERR << "TcpConnection::hanleRead";
    handleError();
  }
}

void TcpConnection::handleClose()
{
  loop_->assertInLoopThread();
  LOG_TRACE << "fd=" << channel_->fd() << " state = " << static_cast<int>(state_);
  assert(state_ == StateE::kConnected || state_ == StateE::kDisconnecting);

  setState(StateE::kDisconnected);
  channel_->disableAll();

  TcpConnectionPtr guardThis(shared_from_this());
  connectionCallback_(guardThis);
  closeCallback_(guardThis);
}

void TcpConnection::handleError()
{
  int err = sockets::getSocketError(channel_->fd());
  LOG_ERROR << "TcpConnection::handleError [" << name_
            << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void TcpConnection::connectionEstablished()
{
  loop_->assertInLoopThread();
  assert(state_ == StateE::kConnecting);
  setState(StateE::kConnected);
  channel_->tie(shared_from_this());
  channel_->enableReading();

  connectionCallback_(shared_from_this());
}

void TcpConnection::connectionDestroyed()
{
  loop_->assertInLoopThread();
  if(state_ == StateE::kConnected)
  {
    setState(StateE::kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());
  }
  channel_->remove();
}