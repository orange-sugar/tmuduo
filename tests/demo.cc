#include "tmuduo/net/sql/sqlConnectionPool.h"

#include "tmuduo/base/Logging.h"

#include "tmuduo/base/AsyncLogging.h"

#include <memory>

using namespace tmuduo;
using namespace tmuduo::net;

// class Test  
// {
//   public:
//     std::unique_ptr<SQLConnectionPool> sqlPool_;
// };

AsyncLogging* g_asynclog;

void logOutput(const char* line, int len)
{
  g_asynclog->append(line, len);
}

int main(int argc, char* argv[])
{
  // Logger::setLogLevel(Logger::TRACE);
  // Test a;
  // a.sqlPool_.reset(&SQLPool::getInstance());
  char name[256] = { '\0' };
  strncpy(name, argv[0], sizeof name - 1);
  AsyncLogging log(::basename(name), 10 * 1000 * 1000, 1);
  g_asynclog = &log;
  if (argc > 1)
  {
    log.start();
    Logger::setOutPut(logOutput);
  }
  Logger::setLogLevel(Logger::DEBUG);
  LOG_DEBUG << "main";
  
}