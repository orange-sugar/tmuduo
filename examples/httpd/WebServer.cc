#include "WebServer.h"
#include <mysql/mysql.h>
#include "tmuduo/base/FileUtil.h"
#include "tmuduo/base/Logging.h"
#include "tmuduo/net/sql/sqlConnectionPool.h"

WebServer::WebServer(EventLoop* loop, const InetAddress& serverAddr, const std::string& nameArg,
                     std::string docRoot, int maxSQLConn)
    : docRoot_(docRoot), server_(loop, serverAddr, nameArg, TcpServer::Option::kReusePort),
      sqlPool_(&SQLPool::getInstance()), filetype_(kUnkown) {
  sqlPool_->init("localhost", "testuser", "testuser123...", "users", maxSQLConn);
  server_.setHttpCallback(
      [&](const HttpRequest& request, HttpResponse* response) { onRequest(request, response); });
  initUserInfo();
}

std::string WebServer::filetypeToString(FileType ft) {
  switch (ft) {
    case kHtml:
      return "text/html";
      break;
    case kImage:
      return "image/png";
      break;
    default:
      return "Unkown";
  }
}

void WebServer::onRequest(const HttpRequest& request, HttpResponse* response) {
  realFile_ = docRoot_;
  if (request.getMethod() == HttpRequest::kGet) {
    // request for root directory
    if (request.getPath() == "/") {
      // realFile_ += "/judge.html";
      realFile_ += "/bench.html";
      filetype_ = kHtml;
    } else if (request.getPath() == "/favion.ico") {
      realFile_ += "/fig.ico";
      filetype_ = kImage;
    } else {
      LOG_DEBUG << "Unknown File Type";
      filetype_ = kUnkown;
      response->setStatusCode(HttpResponse::k404NotFound);
      response->setStatusMessage("Not Found");
      response->setCloseConnection(true);
      return;
    }

    if (filetype_ != kUnkown) {
      LOG_DEBUG << "realfile: " << realFile_;
      FileUtil::ReadSmallFile rsf(realFile_);
      int fileSize;
      int err;
      if ((err = rsf.readToBuffer(&fileSize)) > 0) {
        LOG_ERROR << "ReadSmallFile::readToBuffer";
        response->setStatusCode(HttpResponse::k301MovedPermanently);
        response->setStatusMessage("Moved Permanently");
        response->setCloseConnection(true);
      } else {
        response->setStatusCode(HttpResponse::k200Ok);
        response->setStatusMessage("OK");
        response->setContentType(filetypeToString(filetype_));
        response->addHeader("Server", "orange");
        response->setBody(std::string(rsf.buffer(), fileSize));
        LOG_DEBUG << "Read file success";
      }
    }
  } else if (request.getMethod() == HttpRequest::kPost) {
    LOG_DEBUG << request.getPath();
    if (request.getPath() == "/2" || request.getPath() == "/3") {
      std::string body = request.getBody();
      LOG_DEBUG << "got body: " << body;
      size_t andMark = body.find('&');
      std::string name = body.substr(5, andMark - 5);
      std::string password = body.substr(andMark + 10, body.size() - andMark - 10);
      LOG_DEBUG << "parsed name: " << name << " pswd: " << password;

      // register
      if (request.getPath() == "/3") {
        if (!usersInfo_.count(name)) {
          {
            std::lock_guard lock(mutex_);
            ConnPoolWrapper loop(sqlPool_);
            auto conn = loop.getSQLConnection();
            auto res = conn->query("INSERT INTO users(username, password) values ('" + name +
                                   "', '" + password + "');");
            usersInfo_.emplace(name, password);

            if (!res) {
              realFile_ += "/login.html";
            } else {
              realFile_ += "/registerError.html";
            }
          }
        } else {
          realFile_ += "/registerError.html";
        }
      }
      // log in
      else if (request.getPath() == "/2") {
        // for (const auto& e : usersInfo_)
        // {
        //   printf("name: %s, password: %s\n", e.first.c_str(), e.second.c_str());
        // }
        if (usersInfo_.find(name) != usersInfo_.end() && usersInfo_[name] == password) {
          realFile_ += "/welcome.html";
        } else {
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
    } else {
      if (request.getPath() == "/0") {
        realFile_ += "/register.html";
      } else if (request.getPath() == "/1") {
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

void WebServer::initUserInfo() {
  ConnPoolWrapper pool(sqlPool_);
  auto conn = pool.getSQLConnection();
  auto res = conn->query("SELECT * FROM users;");
  // int numFields = mysql_num_fields(res);
  // MYSQL_FIELD *fields = mysql_fetch_fields(res);
  while (MYSQL_ROW row = mysql_fetch_row(res)) {
    std::string temp1(row[1]);
    std::string temp2(row[2]);
    usersInfo_[temp1] = temp2;
  }
}