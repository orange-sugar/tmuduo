#ifndef TMUDUO_NET_MYHTTPD_HTTPRESPONSE_H
#define TMUDUO_NET_MYHTTPD_HTTPRESPONSE_H

#include <map>
#include "tmuduo/base/Types.h"
#include "tmuduo/base/copyable.h"

namespace tmuduo {
namespace net {

class Buffer;

class HttpResponse : copyable {
 public:
  enum HttpStatusCode {
    kUnknown,
    k200Ok = 200,
    k301MovedPermanently = 301,
    k400BadRequest = 400,
    k404NotFound = 404,
  };

  HttpResponse(bool close) : statusCode_(kUnknown), closeConnection_(close) {
  }

  void setStatusCode(HttpStatusCode code) {
    statusCode_ = code;
  }

  void setStatusMessage(const std::string& message) {
    statusMessage_ = message;
  }

  void setCloseConnection(bool on) {
    closeConnection_ = on;
  }

  bool isCloseConnection() const {
    return closeConnection_;
  }

  void setContentType(const std::string& contentType) {
    addHeader("Content-Type", contentType);
  }

  void addHeader(const std::string& key, const std::string& value) {
    headers_[key] = value;
  }

  void setBody(const std::string& body) {
    body_ = body;
  }

  void appendToBuffer(Buffer* output) const;

 private:
  std::map<std::string, std::string> headers_;

  HttpStatusCode statusCode_;
  std::string statusMessage_;
  bool closeConnection_;
  std::string body_;
};

}  //  namespace net
}  //  namespace tmuduo

#endif  // TMUDUO_NET_MYHTTPD_HTTPRESPONSE_H