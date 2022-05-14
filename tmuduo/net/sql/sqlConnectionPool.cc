#include "tmuduo/net/sql/sqlConnectionPool.h"

#include <mysql/mysql.h>

#include "tmuduo/base/Logging.h"

using namespace tmuduo::net;

SQLConnectionPool::SQLConnectionPool()
{
}

SQLConnectionPool::~SQLConnectionPool()
{
  LOG_DEBUG << "dtor at [" << this << "]";
  destroy();
}

void SQLConnectionPool::init(int maxNum,
                             std::string url,
                             std::string username,
                             std::string password,
                             std::string databaseName)
{
  for (int i = 0; i < maxNum; ++i)
  {
    SQLConnection* conn = new SQLConnection(url, username, password, databaseName);
    if (conn->connect())
    {
      connections_.put(conn);
    }
  }
  establishedNum_ = static_cast<int>(connections_.size());
  maxNum_ = maxNum;  
}

SQLConnection* SQLConnectionPool::takeConnection()
{
  SQLConnection* conn = nullptr;
  conn = connections_.take();
  return conn;
}

bool SQLConnectionPool::putConnection(SQLConnection* conn)
{
  connections_.put(conn);
  return true;
}

void SQLConnectionPool::destroy()
{
  if (connections_.size() > 0)
  {
    while (connections_.size() != 0)
    {
      auto conn = connections_.take();
      delete conn;
    }
    this->establishedNum_ = 0;
  }
}

ConnPoolWrapper::ConnPoolWrapper(SQLConnectionPool* connPool)
{
  conn_ = connPool->takeConnection();
  pool_ = connPool;
}

ConnPoolWrapper::~ConnPoolWrapper()
{
  pool_->putConnection(conn_);
}