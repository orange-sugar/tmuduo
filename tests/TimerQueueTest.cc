#include <stdio.h>
#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/TimerQueue.h"

using namespace tmuduo;
using namespace tmuduo::net;

EventLoop* g_loop;

void print(const char* msg) {
  LOG_INFO << msg;
}

void cancel(TimerId timerId) {
  g_loop->cancel(timerId);
  printf("cancel timerId %ld\n", timerId.sequence());
}

int main() {
  // Logger::setLogLevel(Logger::TRACE);
  EventLoop loop;
  g_loop = &loop;
  LOG_INFO << "main";
  loop.runAfter(1, std::bind(print, "once1"));
  loop.runAfter(2, std::bind(print, "once2"));
  auto t2 = loop.runEvery(2, std::bind(print, "every2"));
  auto t1 = loop.runEvery(3, std::bind(print, "every3"));
  loop.runAfter(6, std::bind(cancel, t1));
  loop.runAfter(10, std::bind(cancel, t2));
  loop.loop();
}