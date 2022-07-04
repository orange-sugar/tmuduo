#include "tmuduo/net/Connector.h"

#include "tmuduo/net/Channel.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/SockOps.h"

using namespace tmuduo;
using namespace tmuduo::net;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
  : loop_(loop),
    serverAddr_(serverAddr),
    connect_(false),
    state_(States::kDisconnected),
    retryDelayMs_(kInitRetryDelayMs)
{
  LOG_DEBUG << "Connector ctor[" << this << "]";
}

Connector::~Connector()
{
  LOG_DEBUG << "Connector dtor[" << this << "]";
  assert(!channel_);
}

void Connector::start()
{
  connect_ = true;
  loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
  loop_->assertInLoopThread();
  assert(state_ == States::kDisconnected);
  if (connect_)
  {
    connect();
  }
  else
  {
    LOG_DEBUG << "do not  connect";
  }
}

void Connector::stop()
{
  connect_ = false;
  loop_->runInLoop(std::bind(&Connector::stopInLoop, this));
  // TODO cancel Timer
}

void Connector::stopInLoop()
{
  loop_->assertInLoopThread();
  if (state_ == States::kConnecting)
  {
    setState(States::kDisconnected);
    int sockfd = removeAndResetChannel();	// 将通道从poller中移除关注，并将channel置空
    retry(sockfd);		// 这里并非要重连，只是调用sockets::close(sockfd);
  }
}

void Connector::connect()
{
  int sockfd = sockets::createNonblockingOrDie(serverAddr_.family());
  int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());
  int savedErrno = (ret == 0) ? 0 : errno;
  switch (savedErrno) {
    case 0:
    case EINPROGRESS:	// 非阻塞套接字，未连接成功返回码是EINPROGRESS表示正在连接
    case EINTR:
    case EISCONN:			// 连接成功
      connecting(sockfd);
      break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
      retry(sockfd);		// 重连
      break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
      LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
      sockets::close(sockfd);	// 不能重连，关闭sockfd
      break;

    default:
      LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
      sockets::close(sockfd);
      // connectErrorCallback_();
      break;
  }
}

void Connector::restart()
{
  loop_->assertInLoopThread();
  setState(States::kDisconnected);
  retryDelayMs_ = kInitRetryDelayMs;
  connect_ = true;
  startInLoop();
}

void Connector::connecting(int sockfd)
{
  setState(States::kConnecting);
  assert(!channel_);
  channel_.reset(new Channel(loop_, sockfd));
  channel_->setWriteCallback(
    std::bind(&Connector::handleWrite, this));
  channel_->setErrorCallback(
    std::bind(&Connector::handleError, this));
  channel_->enableWriting();
}

int Connector::removeAndResetChannel()
{
  channel_->disableAll();
  channel_->remove();	
  int sockfd = channel_->fd();
  // 不能在这里重置channel_，因为正在调用Channel::handleEvent
  loop_->queueInLoop(std::bind(&Connector::resetChannel, this)); // FIXME: unsafe
  return sockfd;
}

void Connector::resetChannel()
{
  channel_.reset();
}

void Connector::handleWrite()
{
  LOG_TRACE << "Connector::handleWrite " << stateToString(state_);
  if (state_ == States::kConnecting)
  {
    int sockfd = removeAndResetChannel();
    int err = sockets::getSocketError(sockfd);
    if (err)
    {
      LOG_WARN << "Connector::handleWrite - SO_ERROR = "
               << err << " " << strerror_tl(err);
      retry(sockfd);
    }
    else if (sockets::isSelfConnect(sockfd))
    {
      LOG_WARN << "Connector::handleWrite - Self connect";
      retry(sockfd);
    }
    else
    {
      setState(States::kConnected);
      if (connect_)
      {
        newConnectionCalback_(sockfd);
      }
      else  
      {
        sockets::close(sockfd);
      }
    }
  }
  else  
  {
    assert(state_ == States::kDisconnected);
  }
}

void Connector::handleError()
{
  LOG_ERROR << "Connector::handleError";
  assert(state_ == States::kConnecting);

  int sockfd = removeAndResetChannel();
  int err = sockets::getSocketError(sockfd);
  LOG_TRACE << "SO_ERROR = " << err << " " << strerror_tl(err);
  retry(sockfd);
}

void Connector::retry(int sockfd)
{
  sockets::close(sockfd);
  setState(States::kDisconnected);
  if (connect_)
  {
    LOG_INFO << "Connector::retry - Retry connecting to " << serverAddr_.toIpPort()
             << " in " << retryDelayMs_ << " milliseconds. ";
    loop_->runAfter(retryDelayMs_/1000.0,
                    std::bind(&Connector::startInLoop, shared_from_this()));
    retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
  }
  else
  {
    LOG_DEBUG << "do not connect";
  }
}

std::string_view Connector::stateToString(States s)
{
  switch (static_cast<int>(s))
  {
    case 0:
      return std::string_view("DisConnected");
      break;
    case 1:
      return std::string_view("DisConnected");
      break;
    case 2:
      return std::string_view("DisConnected");
      break;
    default:
      return std::string_view("none");
      break;
  }
}