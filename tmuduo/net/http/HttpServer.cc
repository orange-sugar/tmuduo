#include "tmuduo/net/http/HttpServer.h"

#include "tmuduo/base/Logging.h"
#include "tmuduo/net/http/HttpContext.h"
#include "tmuduo/net/http/HttpRequest.h"
#include "tmuduo/net/http/HttpResponse.h"

using namespace tmuduo::net;

namespace tmuduo
{
namespace net
{

namespace detail
{
  void defaultHttpCallback(const HttpRequest&, HttpResponse* response)
  {
    response->setStatusCode(HttpResponse::k404NotFound);
    response->setStatusMessage("Not Found");
    response->setCloseConnection(true);
  }
}

HttpServer::HttpServer(EventLoop* loop,
                       const InetAddress& listenAddr,
                       const std::string& name,
                       TcpServer::Option option)
  : server_(loop, listenAddr, name, option),
    httpCallback_(detail::defaultHttpCallback)
{
  server_.setConnectionCallback(
    std::bind(&HttpServer::onConnection, this, _1));
  server_.setMessageCallback(
    std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

void HttpServer::start()
{
  LOG_WARN << "HttpServer[" << server_.name()
    << "] starts listenning on " << server_.hostport();
  server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    conn->setContext(HttpContext());
  }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,
                           Buffer* buf,
                           Timestamp receiveTime)
{
  HttpContext* context = std::any_cast<HttpContext>(conn->getMutableContext());

  if (!context->parseRequest(buf, receiveTime))
  {
    conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
    conn->shutdown();
  }

  if (context->gotAll())
  {
    onRequest(conn, context->getRequest());
    context->reset();
  }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req)
{
  const std::string& connection = req.getHeader("Connection");
  bool close  = connection == "close" ||
        (req.getVersion() == HttpRequest::kHttp1_0 && connection != "Keep-Alive");
  HttpResponse response(close);
  httpCallback_(req, &response);
  Buffer buf;
  response.appendToBuffer(&buf);
  conn->send(std::move(buf));
  if (response.isCloseConnection())
  {
    conn->shutdown();
  }
}

} //  namespace net
} //  namespace tmuduo