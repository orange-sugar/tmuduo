#include "tmuduo/net/Acceptor.h"
#include <fcntl.h>
#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/InetAddress.h"
#include "tmuduo/net/SockOps.h"

using namespace tmuduo;
using namespace tmuduo::net;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
    : loop_(loop), acceptSocket_(sockets::createNonblockingOrDie(listenAddr.family())),
      acceptchannel_(loop, acceptSocket_.fd()), listenning_(false),
      idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
  LOG_TRACE << "Acceptor::ctor, acceptSock->fd=" << acceptSocket_.fd();
  assert(idleFd_ >= 0);
  acceptSocket_.setReuseAddr(true);
  acceptSocket_.setReusePort(reuseport);
  acceptSocket_.bindAddress(listenAddr);
  acceptchannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
  LOG_TRACE << "Acceptor::dtor, acceptSock->fd=" << acceptSocket_.fd();
  acceptchannel_.disableAll();
  acceptchannel_.remove();
  ::close(idleFd_);
}

void Acceptor::listen() {
  loop_->assertInLoopThread();
  listenning_ = true;
  acceptSocket_.listen();
  acceptchannel_.enableReading();
}

void Acceptor::handleRead() {
  loop_->assertInLoopThread();
  InetAddress peerAddr;
  // TODO
  int connfd = acceptSocket_.accept(&peerAddr);
  if (connfd >= 0) {
    if (newConnectionCallback_) {
      newConnectionCallback_(connfd, peerAddr);
    } else {
      sockets::close(connfd);
    }
  } else {
    LOG_SYSERR << "Acceptor::handleRead";
    // Read the section named "The special problem of
    // accept()ing when you can't" in libev's doc.
    // By Marc Lehmann, author of libev.
    if (errno == EMFILE) {
      close(idleFd_);
      idleFd_ = ::accept(acceptSocket_.fd(), nullptr, nullptr);
      ::close(idleFd_);
      idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
}