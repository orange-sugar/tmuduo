#include "tmuduo/net/http/HttpServer.h"

#include "tmuduo/base/FileUtil.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/http/HttpContext.h"
#include "tmuduo/net/http/HttpRequest.h"
#include "tmuduo/net/http/HttpResponse.h"

using namespace tmuduo::net;

class CGIPool : noncopyable
{

};

void onRequest(const HttpRequest& request, HttpResponse* response)
{
  if (request.getMethod() == HttpRequest::kGet)
  {
    if (request.getPath() == "/")
    {
      response->setStatusCode(HttpResponse::k200Ok);
      response->setStatusMessage("OK");
      response->setContentType("test/html");
      response->addHeader("Server", "orange");
    }
    else if (request.getPath() == "/favion.ico")
    {
      ReadSmallFile file()
    }
  }
}

int main()
{
  EventLoop loop;
  InetAddress listenAddr(6912);

  HttpServer server(&loop, listenAddr, "MyHttpd", TcpServer::Option::kReusePort);

  server.setHttpCallback(onRequest);

  server.start();
  loop.loop();
}