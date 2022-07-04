#ifndef TMUDUO_NET_SQL_SQLCONNECTION_H
#define TMUDUO_NET_SQL_SQLCONNECTION_H

#include "tmuduo/base/Types.h"

class MYSQL;
class MYSQL_RES;

namespace tmuduo
{
namespace net
{

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

} // namespace net
} // namespace tmuduo

#endif	// TMUDUO_NET_SQL_SQLCONNECTION_H