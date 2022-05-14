#include <memory>

#include "tmuduo/base/AsyncLogging.h"
#include "tmuduo/base/FileUtil.h"
#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/http/HttpContext.h"
#include "tmuduo/net/http/HttpRequest.h"
#include "tmuduo/net/http/HttpResponse.h"
#include "tmuduo/net/http/HttpServer.h"
#include "tmuduo/net/sql/sqlConnectionPool.h"

#include <mysql/mysql.h>

using namespace tmuduo;
using namespace tmuduo::net;

AsyncLogging* g_asynclog = nullptr;

void output(const char* line, int len)
{
  g_asynclog->append(line, len);
}

class WebServer
{
  public:
    enum FileType {kUnkown, kHtml, kImage};
    WebServer(EventLoop* loop, 
             const InetAddress& serverAddr,
             const std::string& nameArg,
             std::string docRoot,
             int maxSQLConn
             )
      : docRoot_(docRoot),
        server_(loop, serverAddr, nameArg, TcpServer::Option::kReusePort),
        sqlPool_(&SQLPool::getInstance()),
        filetype_(kUnkown)
    {
      sqlPool_->init(maxSQLConn, "localhost", "testuser", "testuser", "users");
      server_.setHttpCallback(
        [&](const HttpRequest &request, HttpResponse *response) 
        {onRequest(request, response);});
      initUserInfo();
    }

    void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }

    void start() { server_.start(); } 

    std::string filetypeToString(FileType ft);   

  private:
    void onRequest(const HttpRequest &request, HttpResponse *response);

    void initUserInfo();

    std::string docRoot_;
    HttpServer server_;
    std::unique_ptr<SQLConnectionPool> sqlPool_;

    std::string realFile_;
    FileType filetype_;

    std::mutex mutex_;
    std::map<std::string, std::string> usersInfo_;
};

int main()
{
  Logger::setLogLevel(Logger::DEBUG);
  AsyncLogging log("httpserver", 400*1000*1000);
  g_asynclog = &log;
  Logger::setOutPut(output);
  log.start();
  
  EventLoop loop;
  InetAddress serverAddr(9981);
  WebServer server(&loop, serverAddr, "Orange Server", "root", 4);
  server.setThreadNum(0);
  server.start();
  loop.loop();
}

void WebServer::onRequest(const HttpRequest &request, HttpResponse *response)
{
  realFile_ = docRoot_;
  if (request.getMethod() == HttpRequest::kGet)
  {
    // request for root directory
    if (request.getPath() == "/")
    {
      realFile_ += "/judge.html";
      filetype_ = kHtml;
    }
    else if (request.getPath() == "/favion.ico")
    {
      realFile_ += "/fig.ico";
      filetype_ = kImage;
    }
    else  
    {
      filetype_ = kUnkown;
      response->setStatusCode(HttpResponse::k404NotFound);
      response->setStatusMessage("Not Found");
      response->setCloseConnection(true);
      return;
    }

    if (filetype_ != kUnkown)
    {
      LOG_DEBUG << "realfile: " << realFile_;
      FileUtil::ReadSmallFile rsf(realFile_);
      int fileSize;
      int err;
      if ((err = rsf.readToBuffer(&fileSize)) > 0)
      {
        LOG_ERROR << "ReadSmallFile::readToBuffer";
        response->setStatusCode(HttpResponse::k301MovedPermanently);
        response->setStatusMessage("Moved Permanently");
        response->setCloseConnection(true);
      }
      else  
      {
        response->setStatusCode(HttpResponse::k200Ok);
        response->setStatusMessage("OK");
        response->setContentType(filetype_ == kHtml ? "text/html" : "image/png");
        response->addHeader("Server", "orange");
        response->setBody(std::string(rsf.buffer(), fileSize));
      } 
    }
  }
  else if (request.getMethod() == HttpRequest::kPost)
  {
    LOG_DEBUG << request.getPath();
    if (request.getPath() == "/2" || request.getPath() == "/3")
    {
      std::string body = request.getBody();
      LOG_DEBUG << "got body: " << body;
      size_t andMark = body.find('&');
      std::string name = body.substr(5, andMark - 5);
      std::string password = body.substr(andMark+10, body.size() - andMark - 10);
      LOG_DEBUG << "parsed name: " << name << " pswd: " << password;
      
      // register
      if (request.getPath() == "/3")
      {
        if (!usersInfo_.count(name))
        {
          {
            std::lock_guard lock(mutex_);
            ConnPoolWrapper loop(sqlPool_.get());
            auto conn = loop.getSQLConnection();
            auto res = conn->query("INSERT INTO users(username, password) values ('"
                        + name +"', '" + password + "');");
            usersInfo_.emplace(name, password);

            if (!res)
            {
              realFile_ += "/login.html";
            }
            else  
            {
              realFile_ += "/registerError.html";
            }
          }
        }
        else  
        {
          realFile_ += "/registerError.html";
        }
      }
      // log in
      else if (request.getPath() == "/2")
      {
        // for (const auto& e : usersInfo_)
        // {
        //   printf("name: %s, password: %s\n", e.first.c_str(), e.second.c_str());
        // }
        if (usersInfo_.find(name) != usersInfo_.end() && usersInfo_[name] == password)
        {
          realFile_ += "/welcome.html";
        }
        else  
        {
          realFile_ += "/loginError.html";
        }
      }
      LOG_DEBUG << "realfile: " << realFile_;
      FileUtil::ReadSmallFile rsf(realFile_);
      int fileSize;
      rsf.readToBuffer(&fileSize);
      response->setStatusCode(HttpResponse::k200Ok);
      response->setStatusMessage("OK");
      response->setContentType("text/html");
      response->addHeader("Server", "orange");
      response->setBody(std::string(rsf.buffer(), fileSize));
      // ConnPoolWrapper pool(sqlPool_.get());
      // auto conn = pool.getSQLConnection();
    }
    else  
    {
      if (request.getPath() == "/0")
      {
        realFile_ += "/register.html";
      }
      else if (request.getPath() == "/1")
      {
        realFile_ += "/login.html";
      }
      LOG_DEBUG << "realfile: " << realFile_;
      FileUtil::ReadSmallFile rsf(realFile_);
      int fileSize;
      rsf.readToBuffer(&fileSize);
      response->setStatusCode(HttpResponse::k200Ok);
      response->setStatusMessage("OK");
      response->setContentType("text/html");
      response->addHeader("Server", "orange");
      response->setBody(std::string(rsf.buffer(), fileSize));
    }  
  }
}

void WebServer::initUserInfo()
{
  ConnPoolWrapper pool(sqlPool_.get());
  auto conn = pool.getSQLConnection();
  auto res = conn->query("SELECT * FROM users;");
  // int numFields = mysql_num_fields(res);
  // MYSQL_FIELD *fields = mysql_fetch_fields(res);
  while (MYSQL_ROW row = mysql_fetch_row(res))
  {
      std::string temp1(row[1]);
      std::string temp2(row[2]);
      usersInfo_[temp1] = temp2;
  }
}