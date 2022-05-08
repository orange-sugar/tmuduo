#ifndef EXAMPLES_HTTPD_SQLCONNECTIONPOOL_H
#define EXAMPLES_HTTPD_SQLCONNECTIONPOOL_H

#include <list>
#include <mutex>
#include <semaphore>

#include "tmuduo/base/Singleton.h"

class MYSQL;

class SQLConnectionPool
{
  public:
    SQLConnectionPool(std::string url, 
                      std::string username,
                      std::string password,
                      std::string databaseName,
                      uint16_t port,
                      int maxNum,
                      bool close
                      );
    ~SQLConnectionPool()
    {
      destroy();
    }
    
    void start();
                    
    MYSQL *getNextConn();

    int freeNum() const { return freeNum_; }

  private:
    std::string url_;
    uint16_t port_;
    std::string username_;
    std::string password_;
    std::string databaseName_;
    bool close_;

  private:
    void destroy();

    int maxNum_;
    int usedNum_;
    int freeNum_;

    std::mutex mutex_;
    // std::condition_variable cond_;

    std::list<MYSQL*> connections_;

};

#endif	// EXAMPLES_HTTPD_SQLCONNECTIONPOOL_H