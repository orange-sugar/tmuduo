#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/EventLoopThread.h"
#include "tmuduo/base/Logging.h"

#include <cstdio>

using namespace tmuduo;
using namespace tmuduo::net;

void runInThread()
{
  printf("runInThread(): pid = %d, tid = %d\n",
         getpid(), CurrentThread::tid());
  LOG_INFO;
}

int main()
{
  // Logger::setLogLevel(Logger::TRACE);
  printf("main(): pid = %d, tid = %d\n",
         getpid(), CurrentThread::tid());

  EventLoopThread loopThread;
  EventLoop* loop = loopThread.startLoop();
  
  loop->runInLoop(runInThread);
  LOG_INFO;
  sleep(1);
  
  loop->runAfter(2, runInThread);
  sleep(3);
  loop->quit();
}