#include "WebServer.h"
#include "tmuduo/base/AsyncLogging.h"
#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"

using namespace tmuduo;
using namespace tmuduo::net;

AsyncLogging* g_asynclog;
EventLoop* g_loop;

void logOutput(const char* line, int len) {
  g_asynclog->append(line, len);
}

void func1() {
  g_loop->quit();
}

int main(int argc, char* argv[]) {
  char name[256] = {'\0'};
  strncpy(name, argv[0], sizeof name - 1);
  AsyncLogging log(::basename(name), 10 * 1000 * 1000, 1);
  g_asynclog = &log;
  if (argc > 1) {
    log.start();
    Logger::setOutPut(logOutput);
  }

  Logger::setLogLevel(Logger::WARN);

  EventLoop loop;
  g_loop = &loop;

  WebServer server(&loop, InetAddress(9981), "WebServer", "root", 4);

  server.setThreadNum(16);
  server.start();
  // loop.runAfter(3, func1);
  loop.loop();
}