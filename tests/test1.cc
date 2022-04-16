#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"

using namespace tmuduo::net;

int main()
{
  Logger::setLogLevel(Logger::DEBUG);
  EventLoop loop;
  loop.loop();
}