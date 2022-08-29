#ifndef TMUDUO_NET_SQL_SQLCONNECTIONPOOL_H
#define TMUDUO_NET_SQL_SQLCONNECTIONPOOL_H

#include <memory>
#include <mutex>
#include "tmuduo/base/BlockingQueue.h"
#include "tmuduo/base/Singleton.h"
#include "tmuduo/net/sql/sqlConnection.h"

namespace tmuduo {
namespace net {

class SQLConnectionPool {
 public:
  SQLConnectionPool();
  ~SQLConnectionPool();

  void init(std::string url, std::string username, std::string password, std::string databaseName,
            int maxNum);

  SQLConnection* takeConnection();
  bool putConnection(SQLConnection*);

  void destroy();

 private:
  int establishedNum_;
  int maxNum_;
  BlockingQueue<SQLConnection*> connections_;
};

class ConnPoolWrapper {
 public:
  ConnPoolWrapper(SQLConnectionPool* connPool);
  // ConnPoolWrapper(SQLConnectionPool *connPool);
  ~ConnPoolWrapper();

  SQLConnection* getSQLConnection() {
    return conn_;
  }

 private:
  SQLConnection* conn_;
  SQLConnectionPool* pool_;
};

using SQLPool = Singleton<SQLConnectionPool>;

}  // namespace net
}  // namespace tmuduo

#endif  // TMUDUO_NET_SQL_SQLCONNECTIONPOOL_H