#ifndef TMUDUO_NET_MYHTTPD_HTTPCONTEXT_H
#define TMUDUO_NET_MYHTTPD_HTTPCONTEXT_H

#include "tmuduo/base/copyable.h"

#include "tmuduo/net/http/HttpRequest.h"

namespace tmuduo
{
namespace net
{

class Buffer;

class HttpContext : public copyable
{
  public:
    // states of parsing 
    enum HttpRequestParseState
    {
      kExpectRequestLine,
      kExpectHeaders,
      kExpectBody,
      kGotAll
    };

    HttpContext() : state_(kExpectRequestLine)
    { }

    // to parse a request from Buffer
    // return false on error
    bool parseRequest(Buffer* buf, Timestamp receiveTime);

    bool gotAll() const { return state_ == kGotAll; }

    void reset()
    {
      state_ = kExpectRequestLine;
      HttpRequest dummy;
      request_.swap(dummy);
    }

    const HttpRequest& getRequest() const { return request_; }
    HttpRequest& getRequest() { return request_; }

  private:
    // parse the first line of a http request like
    // GET http://xxx.com?xxxx HTTP/1.1
    bool processRequestLine(const char* start, const char* end);

    HttpRequestParseState state_;
    HttpRequest request_;
};

} //  namespace net
} //  namespace tmuduo

#endif	// TMUDUO_NET_MYHTTPD_HTTPCONTEXT_H