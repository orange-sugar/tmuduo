#include "tmuduo/net/TcpConnection.h"



#include "tmuduo/base/Logging.h"
#include "tmuduo/net/Channel.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/Socket.h"

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
  // 尚未加入对连接关闭的处理
  loop_->assertInLoopThread();
  char buf[65536];
  ssize_t n = ::read(channel_->fd(), buf, sizeof(buf));
  messageCallback_(shared_from_this(), buf, n);
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