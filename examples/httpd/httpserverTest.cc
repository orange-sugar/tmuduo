#include "tmuduo/net/http/HttpServer.h"

#include "tmuduo/base/FileUtil.h"
#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/http/HttpContext.h"
#include "tmuduo/net/http/HttpRequest.h"
#include "tmuduo/net/http/HttpResponse.h"

#include <mysql/mysql.h>

using namespace tmuduo;
using namespace tmuduo::net;

class CGIPool : noncopyable {};

struct userItem {
  std::string userid_;
  std::string username_;
  std::string password_;
};

void onRequest(const HttpRequest &request, HttpResponse *response) {
  // LOG_INFO << "headers total " << request.headers().size();
  if (request.getMethod() == HttpRequest::kGet) {
    if (request.getPath() == "/") {
      response->setStatusCode(HttpResponse::k200Ok);
      response->setStatusMessage("OK");
      response->setContentType("text/html");
      response->addHeader("Server", "orange");
      std::string now = Timestamp::now().toFormattedString();
      FileUtil::ReadSmallFile rf("load.html");
      int sz;
      rf.readToBuffer(&sz);
      response->setBody(std::string(rf.buffer(), sz));
    } else if (request.getPath() == "/favion.ico") {
      FileUtil::ReadSmallFile rf("fig.ico");
      int figSize;
      rf.readToBuffer(&figSize);
      response->setStatusCode(HttpResponse::k200Ok);
      response->setStatusMessage("OK");
      response->setContentType("image/png");

      response->setBody(std::string(rf.buffer(), figSize));
      // response->setBody(std::string(favicon, sizeof favicon));
    } else {
      response->setStatusCode(HttpResponse::k404NotFound);
      response->setStatusMessage("Not Found");
      response->setCloseConnection(true);
    }
  } else if (request.getMethod() == HttpRequest::kPost) {
    // LOG_INFO << "request body: " << request.getBody();
    response->setStatusCode(HttpResponse::k200Ok);
    response->setStatusMessage("OK");
    response->setContentType("text/html");
    response->addHeader("Server", "orange");

    // get username
    std::string body = request.getBody();
    size_t andMark = body.find('&');
    size_t equal = body.find('=');
    std::string username = body.substr(equal + 1, andMark - equal - 1);
    // equal = body.find('=');

    MYSQL sqlConn;
    if (!mysql_init(&sqlConn)) {
      LOG_SYSERR << "mysql init";
    }
    if (!mysql_real_connect(&sqlConn, "127.0.0.1", "testuser", "testuser",
                            "users", 3306, nullptr, 0)) {
      LOG_SYSERR << "mysql_connect";
    }

    MYSQL_RES *res;

    std::string sqlQuery("select * from users.users");
    sqlQuery += " where username = \"" + username + "\";";

    userItem item;

    mysql_query(&sqlConn, sqlQuery.c_str());
    res = mysql_store_result(&sqlConn);
    if (res) {
      auto numRow = mysql_num_rows(res);
      // auto numCol = mysql_num_fields(res);
      assert(numRow == 1);
      auto row = mysql_fetch_row(res);
      item.userid_ = row[0];
      item.username_ = row[1];
      item.password_ = row[2];

      std::string respBody = R"(<! DOCTYPE HTML>
                          <html>
                            <head>
                              <title>response</title>
                            </head>     
                            <body>)";
      respBody += "hello! our No." + item.userid_ + " user " + item.username_ +
                  ", your password is " + item.password_;
      respBody += R"(   </body>
          </html>)";
      response->setBody(respBody);
    } else {
      response->setStatusCode(HttpResponse::k200Ok);
      response->setStatusMessage("OK");
      response->setContentType("text/html");
      response->addHeader("Server", "orange");
      std::string respBody = R"(<! DOCTYPE HTML>
                          <html>
                            <head>
                              <title>response</title>
                            </head>     
                            <body>)";
      respBody += "Sorry, you are not in our database.";
      respBody += R"(   </body>
          </html>)";
      response->setBody(respBody);
    }

    mysql_free_result(res);
    mysql_close(&sqlConn);
  }
}

int main(int argc, char *argv[]) {
  // bool benchmark = false;
  if (argc > 1) {
    // benchmark = true;
    Logger::setLogLevel(Logger::WARN);
  }
  Logger::setLogLevel(Logger::INFO);
  EventLoop loop;
  InetAddress listenAddr(9981);

  HttpServer server(&loop, listenAddr, "MyHttpd",
                    TcpServer::Option::kReusePort);

  server.setHttpCallback(onRequest);
  server.setThreadNum(4);
  server.start();
  loop.loop();
}
