#include "tmuduo/net/sql/sqlConnection.h"
#include <mysql/mysql.h>
#include "tmuduo/base/Logging.h"

using namespace tmuduo::net;

SQLConnection::SQLConnection(std::string url, std::string username, std::string password,
                             std::string databaseName)
    : url_(url), username_(username), password_(password), databaseName_(databaseName),
      conn_(nullptr) {
  LOG_DEBUG << "ctor at [" << this << "]";
  conn_ = mysql_init(conn_);
}

SQLConnection::~SQLConnection() {
  LOG_DEBUG << "dtor at [" << this << "]";
  if (!conn_) {
    mysql_close(conn_);
  }
}

bool SQLConnection::connect() {
  conn_ = mysql_real_connect(conn_, url_.c_str(), username_.c_str(), password_.c_str(),
                             databaseName_.c_str(), 3306, nullptr, 0);
  if (conn_ == nullptr) {
    // LOG_TRACE;
    LOG_ERROR << url_ << username_ << password_ << databaseName_;
    LOG_ERROR << "connect";
    return false;
  }
  return true;
}

bool SQLConnection::update(std::string sql) {
  if (mysql_query(conn_, sql.c_str())) {
    LOG_ERROR << "update";
    return false;
  }
  return true;
}

MYSQL_RES* SQLConnection::query(std::string sql) {
  LOG_DEBUG << sql;
  if (mysql_query(conn_, sql.c_str())) {
    LOG_ERROR << "query";
    return nullptr;
  }
  return mysql_use_result(conn_);
}