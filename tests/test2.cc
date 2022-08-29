#include <sys/timerfd.h>
#include <cstdio>
#include <cstdlib>
#include "tmuduo/base/Logging.h"
#include "tmuduo/net/Channel.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/Poller.h"

using namespace tmuduo;
using namespace tmuduo::net;

EventLoop* g_loop;
int timerfd;

class setENV {
 public:
  setENV() {
    ::setenv("TMUDUO_USE_POLL", "ON", 1);
  }
  ~setENV() {
    ::unsetenv("TMUDUO_USE_POLL");
  }
};

// setENV setUSEPOLL;

void print() {
  printf("timeout! tid = %d\n", CurrentThread::tid());
  uint64_t howmany;
  ::read(timerfd, &howmany, sizeof(howmany));
  g_loop->quit();
}

const char* ENV = "TMUDUO_USE_POLL";

int main() {
  Logger::setLogLevel(Logger::TRACE);
  EventLoop loop;
  g_loop = &loop;

  timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  Channel channel(&loop, timerfd);
  channel.setReadCallback(std::bind(print));
  channel.enableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof(howlong));
  howlong.it_value.tv_sec = 5;
  ::timerfd_settime(timerfd, 0, &howlong, NULL);

  loop.loop();
  ::close(timerfd);
}