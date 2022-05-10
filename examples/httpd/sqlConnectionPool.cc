#include "sqlConnectionPool.h"

#include <mysql/mysql.h>

#include "tmuduo/base/Logging.h"

using namespace tmuduo;

SQLConnection::SQLConnection(std::string url,
                             std::string username,
                             std::string password,
                             std::string databaseName)
  : url_(url),
    username_(username),
    password_(password),
    databaseName_(databaseName),
    conn_(nullptr)
{
  // LOG_DEBUG << "ctor at [" << this << "]";
  conn_ = mysql_init(conn_);
}   

SQLConnection::~SQLConnection()
{
  // LOG_DEBUG << "dtor at [" << this << "]";
  if (!conn_)
  {
    mysql_close(conn_);
  }
}

bool SQLConnection::connect()
{
  conn_ = mysql_real_connect(conn_,
                              url_.c_str(),
                              username_.c_str(),
                            password_.c_str(),
                               databaseName_.c_str(),
                            3306,
                            nullptr, 0);
  return conn_ != nullptr;
  
}

bool SQLConnection::update(std::string sql)
{
  if (mysql_query(conn_, sql.c_str()))
  {
    LOG_ERROR << "update";
    return false;
  }
  return true;
}

MYSQL_RES* SQLConnection::query(std::string sql)
{
  if (mysql_query(conn_, sql.c_str()))
  {
    LOG_ERROR << "query";
    return nullptr;
  }
  return mysql_use_result(conn_);
}

SQLConnectionPool::SQLConnectionPool()
{
}

SQLConnectionPool::~SQLConnectionPool()
{
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
  // if (connections_.size() != 0)
  // {
  //   conn = connections_.take();
  //   return conn;
  // }
  // else
  // {
  //   return nullptr;
  // }
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