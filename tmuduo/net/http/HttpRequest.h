#ifndef TMUDUO_NET_HTTP_HTTPREQUEST_H
#define TMUDUO_NET_HTTP_HTTPREQUEST_H

#include <assert.h>
#include <stdio.h>
#include <map>

#include "tmuduo/base/copyable.h"
#include "tmuduo/base/Timestamp.h"
#include "tmuduo/base/Types.h"

namespace tmuduo
{
namespace net
{

class HttpRequest : public copyable
{
  public:
    enum Method
    {
      kInvalid, kGet, kPost, kHead, kPut, kDelete
    };
    enum Version
    {
      kUnknown, kHttp1_0, kHttp1_1
    };

    HttpRequest() : method_(kInvalid), version_(kUnknown)
    { }

    void setVersion(Version v) { version_ = v; }

    Version getVersion() const { return version_; }

    bool setMethod(const char* start, const char* end)
    {
      assert(method_ == kInvalid);
      std::string_view m(start, static_cast<size_t>(end - start));
      if (m == "GET")
      {
        method_ = kGet;
      }
      else if (m == "POST")
      {
        method_ = kPost;
      }
      else if (m == "HEAD")
      {
        method_ = kHead;
      }
      else if (m == "PUT")
      {
        method_ = kPut;
      }
      else if (m == "DELETE")
      {
        method_ = kDelete;
      }
      else
      {
        method_ = kInvalid;
      }
      return method_ != kInvalid;
    }

    Method method() const { return method_; }

    const std::string_view methodString() const
    {
      switch (method_) {
        case kGet:
          return std::string_view{"GET"};
        case kPost:
          return std::string_view{"POST"};
        case kHead:
          return std::string_view{"HEAD"};
        case kPut:
          return std::string_view{"PUT"};
        case kDelete:
          return std::string_view{"DELETE"};
        default:
          return std::string_view{"UNKNOWN"};
      }
    }

    void setPath(const char* start, const char* end)
    {
      path_.assign(start, end);
    }

    const std::string& path() const { return path_; }

    void setQuery(const char* start, const char* end)
    {
      query_.assign(start, end);
    }

    const std::string query() const { return query_; }

    void setReceiveTime(Timestamp t) { receiveTime_ = t; }

    Timestamp receiveTime() const { return receiveTime_; }

    void addHeader(const char* start, const char* colon, const char* end)
    {
      std::string field(start, colon);
      ++colon;
      while (colon < end && isspace(*colon))
      {
        ++colon;
      }
      std::string value(colon, end);
      while (!value.empty() && isspace(value[value.size()-1]))
      {
        value.resize(value.size() - 1);
      }
      headers_[field] = value;
    }

    std::string getHeader(const std::string& field) const
    {
      std::string result;
      const auto it = headers_.find(field);
      if (it != headers_.end())
      {
        result = it->second;
      }
      return result;
    }

    const std::map<std::string, std::string>& headers() const
    { return headers_; }

    void swap(HttpRequest& rhs)
    {
      std::swap(method_, rhs.method_);
      std::swap(version_, rhs.version_);
      path_.swap(rhs.path_);
      query_.swap(rhs.query_);
      receiveTime_.swap(rhs.receiveTime_);
      headers_.swap(rhs.headers_);
    }

  private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;
    Timestamp receiveTime_;
    std::map<std::string, std::string> headers_;
};

} //  namespace net
} //  namespace tmuduo

#endif	// TMUDUO_NET_HTTP_HTTPREQUEST_H