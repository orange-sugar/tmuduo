#include "sqlConnectionPool.h"

#include <mysql/mysql.h>

#include "tmuduo/base/Logging.h"

using namespace tmuduo;

SQLConnectionPool::SQLConnectionPool(std::string url, 
                      std::string username,
                      std::string password,
                      std::string databaseName,
                      uint16_t port,
                      int maxNum,
                      bool close
                      )
  : url_(url),
    port_(port),
    username_(username),
    password_(password),
    databaseName_(databaseName),
    close_(close),
    maxNum_(maxNum),
    usedNum_(0),
    freeNum_(0)
{

}

void SQLConnectionPool::destroy()
{
  std::scoped_lock lock(mutex_);
  if (connections_.size() > 0)
  {
    for (auto &conn : connections_)
    {
      mysql_close(conn);
    }
    usedNum_ = 0;
    freeNum_ = 0;
  }
}

void SQLConnectionPool::start()
{
  for (int i = 0; i < maxNum_; ++i)
  {
    MYSQL* conn = nullptr;
    conn = mysql_init(conn);

    if (conn == nullptr)
    {
      LOG_ERROR << "MySQL init Error";
      exit(1);
    }
    conn = mysql_real_connect(conn,
                              url_.c_str(),
                              username_.c_str(),
                              password_.c_str(),
                              databaseName_.c_str(),
                              port_,
                              nullptr,
                              0);
    if (conn == nullptr)
    {
      LOG_ERROR << "MySQL real connect Error";
      exit(1);
    }
    connections_.push_back(conn);
    ++freeNum_;
  }
  maxNum_ = freeNum_;
}

MYSQL* SQLConnectionPool::getNextConn()
{
  MYSQL* conn = nullptr;
  if (connections_.size() == 0)
  {
    return nullptr;
  }

  cond_.wait()
}