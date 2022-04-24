#include "tmuduo/net/TcpConnection.h"



#include "tmuduo/base/Logging.h"
#include "tmuduo/net/Channel.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/Socket.h"
#include "tmuduo/net/SockOps.h"

using namespace tmuduo::net;

void tmuduo::net::defaultConnectionCallback(const TcpConnectionPtr& conn)
{
  LOG_TRACE << conn->localAddress().toIpPort() << " -> "
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");
}

void tmuduo::net::defaultMessageCallback(const TcpConnectionPtr& conn,
                                         Buffer* buf,
                                         Timestamp)
{
  buf->retrieveAll();
}

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
  channel_->setCloseCallback(
    std::bind(&TcpConnection::handleClose, this)
  );
  channel_->setErrorCallback(
    std::bind(&TcpConnection::handleError, this)
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

void TcpConnection::send(const void* message, size_t len)
{
  if (state_ == StateE::kConnected)
  {
    if (loop_->isInLoopThread())
    {
      sendInLoop(message, len);
    }
    else  
    {
      std::string_view data(static_cast<const char*>(message), len);
      loop_->runInLoop(
        [&] {
          sendInLoop(data);
        }
      );
    }
  }
}
void TcpConnection::send(const std::string_view& message)
{
  if (state_ == StateE::kConnected)
  {
    if (loop_->isInLoopThread())
    {
      sendInLoop(message);
    }
    else  
    {
      loop_->runInLoop(
        [&] { sendInLoop(message); }
      );
    }
  }
}
void TcpConnection::send(Buffer&& message)
{
  if (state_ == StateE::kConnected)
  {
    if (loop_->isInLoopThread())
    {
      sendInLoop(message.peek(), message.readableBytes());
      message.retrieveAll();
    }
    else  
    {
      loop_->runInLoop(
        [&] { sendInLoop(message.retrieveAllAsString()); }
      );
    }
  }
}

void TcpConnection::sendInLoop(const std::string_view& message)
{
  sendInLoop(message.data(), message.size());
}
void TcpConnection::sendInLoop(const void* message, size_t len)
{
  loop_->assertInLoopThread();
  sockets::write(channel_->fd(), message, len);
}

void TcpConnection::shutdown()
{
  if (state_.exchange(StateE::kDisconnecting) == StateE::kConnected)
  {
    loop_->runInLoop(
      std::bind(&TcpConnection::shutdownInLoop, this)
    );
  }
}

void TcpConnection::shutdownInLoop()
{
  loop_->assertInLoopThread();
  if (!channel_->isWriting())
  {
    socket_->shutdownWrite();
  }
}

void TcpConnection::setTcpNoDelay(bool on)
{
  socket_->setTcpNoDelay(on);
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
  loop_->assertInLoopThread();
  int savedErrno = 0;
  ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
  if (n > 0)
  {
    messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
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
  LOG_TRACE << "fd=" << channel_->fd() << " state = " << static_cast<int>(state_.load());
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