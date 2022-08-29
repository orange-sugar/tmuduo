#ifndef TMUDUO_NET_MYHTTPD_HTTPREQUEST_H
#define TMUDUO_NET_MYHTTPD_HTTPREQUEST_H

#include <assert.h>
#include <map>
#include "tmuduo/base/Timestamp.h"
#include "tmuduo/base/Types.h"
#include "tmuduo/base/copyable.h"

namespace tmuduo {
namespace net {

class HttpRequest : public copyable {
 public:
  // http method
  enum Method { kInvalid, kGet, kPost, kHead, kPut, kDelete };

  // http version
  enum Version { kUnknown, kHttp1_0, kHttp1_1 };

  HttpRequest() : method_(kInvalid), version_(kUnknown) {
  }

  // setter and getter for version_
  void setVersion(Version v) {
    version_ = v;
  }
  Version getVersion() const {
    return version_;
  }

  // setter of method
  bool setMethod(const char* start, const char* end) {
    assert(method_ == kInvalid);
    std::string m(start, static_cast<size_t>(end - start));
    if (m == "GET") {
      method_ = kGet;
    } else if (m == "POST") {
      method_ = kPost;
    } else if (m == "HEAD") {
      method_ = kHead;
    } else if (m == "PUT") {
      method_ = kPut;
    } else if (m == "DELETE") {
      method_ = kDelete;
    } else {
      method_ = kInvalid;
    }
    return method_ != kInvalid;
  }
  // getter of method
  Method getMethod() const {
    return method_;
  }
  // convert Method to string
  const std::string methodToString() const {
    switch (method_) {
      case kGet:
        return std::string{"GET"};
      case kPost:
        return std::string{"POST"};
      case kHead:
        return std::string{"HEAD"};
      case kPut:
        return std::string{"PUT"};
      case kDelete:
        return std::string{"DELETE"};
      default:
        return std::string{"UNKNOWN"};
    }
  }

  // set the path
  void setPath(const char* start, const char* end) {
    path_.assign(start, end);
  }
  // get the path
  const std::string getPath() const {
    return path_;
  }

  //set the query
  void setQuery(const char* start, const char* end) {
    query_.assign(start, end);
  }
  // get the query
  const std::string getQquery() const {
    return query_;
  }

  // set the receiveTime
  void setReceiveTime(Timestamp t) {
    receiveTime_ = t;
  }
  // get the receiveTime
  Timestamp getReceiveTime() const {
    return receiveTime_;
  }

  // add one header to request
  // a typical header is formatted as
  // fieldname:fieldvalue
  void addHeader(const char* start, const char* colon, const char* end) {
    std::string fieldName(start, colon);
    ++colon;
    for (; colon < end && !isspace(*colon); ++colon)
      ;
    std::string fieldValue(colon, end);
    while (!fieldValue.empty() && isspace(fieldValue[fieldValue.size() - 1])) {
      fieldValue.resize(fieldValue.size() - 1);
    }
    headers_[fieldName] = fieldValue;
  }

  // get a field value of a field name
  std::string getHeader(std::string fieldName) const {
    const auto it = headers_.find(fieldName);
    return it == headers_.end() ? std::string{} : it->second;
  }

  const std::map<std::string, std::string>& headers() const {
    return headers_;
  }

  void setBody(const char* start, const char* end) {
    body_ = std::string(start, end);
  }

  std::string getBody() const {
    return body_;
  }

  void swap(HttpRequest& rhs) {
    std::swap(method_, rhs.method_);
    std::swap(version_, rhs.version_);
    path_.swap(rhs.path_);
    query_.swap(rhs.query_);
    receiveTime_.swap(rhs.receiveTime_);
    headers_.swap(rhs.headers_);
    body_.swap(rhs.body_);
  }

 private:
  Method method_;
  Version version_;
  std::string path_;
  std::string query_;
  Timestamp receiveTime_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};

}  //  namespace net
}  //  namespace tmuduo

#endif  // TMUDUO_NET_MYHTTPD_HTTPREQUEST_H