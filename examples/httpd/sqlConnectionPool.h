#ifndef EXAMPLES_HTTPD_SQLCONNECTIONPOOL_H
#define EXAMPLES_HTTPD_SQLCONNECTIONPOOL_H

#include <memory>
#include <mutex>
#include <semaphore>

#include "tmuduo/base/BlockingQueue.h"
#include "tmuduo/base/Singleton.h"

using namespace tmuduo;

class MYSQL;
class MYSQL_RES;

class SQLConnection
{
  public:
    SQLConnection(std::string url,
                  std::string username,
                  std::string password,
                  std::string databaseName);
    ~SQLConnection();

    bool connect();

    bool update(std::string sql);

    MYSQL_RES* query(std::string sql);

    MYSQL* getConnection() { return conn_; } 
  
  private:
    std::string url_;
    std::string username_;
    std::string password_;
    std::string databaseName_;
    MYSQL* conn_;
};

class SQLConnectionPool
{
  public:
    // SQLConnectionPool(int maxNum,
    //                   std::string url,
    //                   std::string username,
    //                   std::string password,
    //                   std::string databaseName);
    SQLConnectionPool();
    ~SQLConnectionPool();

    void init(int maxNum,
              std::string url,
              std::string username,
              std::string password,
              std::string databaseName);

    SQLConnection* takeConnection();
    bool putConnection(SQLConnection*);

    void destroy();

  private:
    int establishedNum_;
    int maxNum_;
    BlockingQueue<SQLConnection*> connections_;
};

class ConnPoolWrapper
{
  public:
    ConnPoolWrapper(SQLConnectionPool *connPool);
    ~ConnPoolWrapper();

    SQLConnection* getSQLConnection() { return conn_; }

  private:
    SQLConnection* conn_;
    SQLConnectionPool *pool_;
};

#endif	// EXAMPLES_HTTPD_SQLCONNECTIONPOOL_H