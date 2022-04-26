#ifndef TMUDUO_NET_HTTP_HTTPCONNEXT_H
#define TMUDUO_NET_HTTP_HTTPCONNEXT_H

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
    enum HttpRequestParseState
    {
      kExpectRequestLine,
      kExpectHeaders,
      kExpectBody,
      kGotAll
    };

    HttpContext() : state_(kExpectRequestLine)
    { }

    // return false on error
    bool parseRequest(Buffer* buf, Timestamp receiveTime);

    bool gotAll() const { return state_ == kGotAll; }

    void reset()
    {
      state_ = kExpectRequestLine;
      HttpRequest dummy;
      request_.swap(dummy);
    }

    const HttpRequest& request() const { return request_; }
    HttpRequest& request() { return request_; }
  
  private:
    bool processRequestLine(const char* begin, const char* end);

    HttpRequestParseState state_;
    HttpRequest request_;
};

} // namespace net
} // namespace tmuduo


#endif	// TMUDUO_NET_HTTP_HTTPCONNEXT_H