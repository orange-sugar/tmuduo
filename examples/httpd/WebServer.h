#ifndef EXAMPLES_HTTPD_WEBSERVER_H
#define EXAMPLES_HTTPD_WEBSERVER_H

#include <memory>

#include "tmuduo/net/http/HttpContext.h"
#include "tmuduo/net/http/HttpRequest.h"
#include "tmuduo/net/http/HttpResponse.h"
#include "tmuduo/net/http/HttpServer.h"
#include "tmuduo/net/sql/sqlConnectionPool.h"

using namespace tmuduo;
using namespace tmuduo::net;

class WebServer
{
  public:
    enum FileType {kUnkown, kHtml, kImage};
    WebServer(EventLoop* loop, 
             const InetAddress& serverAddr,
             const std::string& nameArg,
             std::string docRoot,
             int maxSQLConn
             );

    void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }

    void start() { server_.start(); } 

    std::string filetypeToString(FileType ft);   

  private:
    void onRequest(const HttpRequest &request, HttpResponse *response);

    void initUserInfo();

    std::string docRoot_;
    HttpServer server_;
    // ERROR std::unique_ptr<SQLConnectionPool> sqlPool_;
    SQLConnectionPool* sqlPool_;

    std::string realFile_;
    FileType filetype_;

    std::mutex mutex_;
    std::map<std::string, std::string> usersInfo_;
};

#endif	// EXAMPLES_HTTPD_WEBSERVER_H