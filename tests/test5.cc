#include <cstdio>
#include <thread>
#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"

using namespace tmuduo;
using namespace tmuduo::net;

EventLoop* g_loop;

void print() {
  printf("time out\n");
}

void threadFunc() {
  g_loop->runAfter(2, print);
}

int main() {
  EventLoop loop;
  g_loop = &loop;
  std::thread t(threadFunc);
  loop.loop();
  t.join();
}