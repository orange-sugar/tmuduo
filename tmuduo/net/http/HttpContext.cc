#include "tmuduo/net/http/HttpContext.h"

#include "tmuduo/net/Buffer.h"

using namespace tmuduo;
using namespace tmuduo::net;

bool HttpContext::processRequestLine(const char* start, const char* end)
{
  bool succeed = false;
  const char* begin = start;
  const char* space = std::find(begin, end, ' ');
  if (space != end && request_.setMethod(begin, space))
  {
    begin = space + 1;
    space = std::find(begin, end, ' ');
    if (space != end)
    {
      const char* question = std::find(begin, space, '?');
      // has a query
      if (question != space)
      {
        request_.setPath(begin, question);
        request_.setQuery(question, space);
      }
      else  
      {
        request_.setPath(begin, space);
      }
      begin = space + 1;
      succeed = end - begin == 8 && std::equal(begin, end-1, "HTTP/1.");
      
      // only HTTP/1.0 and HTTP/1.1 support
      if (succeed)
      {
        if (*(end-1) == '1')
        {
          request_.setVersion(HttpRequest::kHttp1_1);
        }
        else if (*(end-1) == '0')
        {
          request_.setVersion(HttpRequest::kHttp1_0);
        }
        else  
        {
          succeed = false;
        }
      }
    }
  }
  return succeed;
}

bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
  bool ok = true;
  bool hasMore = true;
  while (hasMore)
  {
    if (state_ == kExpectRequestLine)
    {
      const char* crlf = buf->findCRLF();
      if (crlf)
      {
        ok = processRequestLine(buf->peek(), crlf);
        if (ok)
        {
          request_.setReceiveTime(receiveTime);
          buf->retrieveUntil(crlf+2);
          state_ = kExpectHeaders;
        }
        else  
        {
          hasMore = false;
        }
      }
      else  
      {
        hasMore = false;
      }
    }
    else if (state_ == kExpectHeaders)
    {
      const char* crlf = buf->findCRLF();
      if (crlf)
      {
        const char* colon = std::find(buf->peek(), crlf, ':');
        if (colon != crlf)
        {
          request_.addHeader(buf->peek(), colon, crlf);
        }
        else  
        {
          // TODO set to kExpectBody
          state_ = kGotAll;
          hasMore = false;
        }
        buf->retrieveUntil(crlf + 2);
      }
      else  
      {
        hasMore = false;
      }
    }
    else if (state_ == kExpectBody)
    {

    }
  }
  return ok;
}