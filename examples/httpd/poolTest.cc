#include "sqlConnectionPool.h"
#include "tmuduo/base/Logging.h"
#include "tmuduo/base/Singleton.h"

#include <thread>

#include <mysql/mysql.h>

using namespace tmuduo;

using connPool = Singleton<SQLConnectionPool>;

void threadFunc()
{
  LOG_DEBUG << "instance at [" << &(connPool::getInstance()) << "]";
  SQLConnection* conn;
  // loop util acquiring a conn
  while ((conn = connPool::getInstance().takeConnection()) == nullptr);
  
  MYSQL_RES* res = conn->query("select * from users.users;");
  auto row = mysql_fetch_row(res);
  LOG_INFO << row[0];
  mysql_free_result(res);
  connPool::getInstance().putConnection(conn);
}

void threadFunc2()
{
  // SQLConnection* conn = nullptr;
  SQLConnection* conn;
  MYSQL_RES* res = nullptr;

  ConnPoolWrapper pool(&connPool::getInstance());

  conn = pool.getSQLConnection();

  res = conn->query("select * from users.users;");

  auto row = mysql_fetch_row(res);
  LOG_INFO << row[0] << " "
           << row[1] << " " 
           << row[2];
  mysql_free_result(res);
}

int main()
{ 
  Logger::setLogLevel(Logger::DEBUG);

  connPool::getInstance().init(5, "127.0.0.1", "testuser", "testuser", "users");
  LOG_DEBUG << "instance at [" << &(connPool::getInstance()) << "]";

  int threadNum = 10;
  std::unique_ptr<std::thread[]> threads;
  threads.reset(new std::thread[threadNum]);

  for (int i = 0; i < threadNum; ++i)
  {
    threads[i] = std::thread(threadFunc2);
  }

  for (int i = 0; i < threadNum; ++i)
  {
    threads[i].join();
  }  
}