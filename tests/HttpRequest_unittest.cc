#include <gtest/gtest.h>
#include "tmuduo/net/Buffer.h"
#include "tmuduo/net/http/HttpContext.h"

using namespace tmuduo;
using std::string;
using tmuduo::net::Buffer;
using tmuduo::net::HttpContext;
using tmuduo::net::HttpRequest;

TEST(HttpRequestTest, testParseRequestALlInOne) {
  HttpContext context;
  Buffer input;
  input.append(
      "GET /index.html HTTP/1.1\r\n"
      "Host: www.baidu.com\r\n"
      "\r\n");
  ASSERT_TRUE(context.parseRequest(&input, Timestamp::now()));
  ASSERT_TRUE(context.gotAll());
  const HttpRequest& req = context.getRequest();
  EXPECT_EQ(req.getMethod(), HttpRequest::kGet);
  EXPECT_EQ(req.getPath(), string("/index.html"));
  EXPECT_EQ(req.getVersion(), HttpRequest::kHttp1_1);
  EXPECT_EQ(req.getHeader("Host"), string("www.baidu.com"));
  EXPECT_EQ(req.getHeader("User-Agent"), string(""));
}

TEST(HttpRequestTest, testParseRequestInTwoPieces) {
  string all(
      "GET /index.html HTTP/1.1\r\n"
      "Host: www.baidu.com\r\n"
      "\r\n");

  for (size_t sz1 = 0; sz1 < all.size(); ++sz1) {
    HttpContext context;
    Buffer input;
    input.append(all.c_str(), sz1);
    ASSERT_TRUE(context.parseRequest(&input, Timestamp::now()));
    ASSERT_TRUE(!context.gotAll());

    size_t sz2 = all.size() - sz1;
    input.append(all.c_str() + sz1, sz2);
    ASSERT_TRUE(context.parseRequest(&input, Timestamp::now()));
    ASSERT_TRUE(context.gotAll());
    const HttpRequest& request = context.getRequest();
    EXPECT_EQ(request.getMethod(), HttpRequest::kGet);
    EXPECT_EQ(request.getPath(), string("/index.html"));
    EXPECT_EQ(request.getVersion(), HttpRequest::kHttp1_1);
    EXPECT_EQ(request.getHeader("Host"), string("www.baidu.com"));
    EXPECT_EQ(request.getHeader("User-Agent"), string(""));
  }
}

TEST(HttpRequestTest, testParseRequestEmptyHeaderValue) {
  HttpContext context;
  Buffer input;
  input.append(
      "GET /index.html HTTP/1.1\r\n"
      "Host: www.baidu.com\r\n"
      "User-Agent:\r\n"
      "Accept-Encoding: \r\n"
      "\r\n");

  ASSERT_TRUE(context.parseRequest(&input, Timestamp::now()));
  ASSERT_TRUE(context.gotAll());
  const HttpRequest& request = context.getRequest();
  EXPECT_EQ(request.getMethod(), HttpRequest::kGet);
  EXPECT_EQ(request.getPath(), string("/index.html"));
  EXPECT_EQ(request.getVersion(), HttpRequest::kHttp1_1);
  EXPECT_EQ(request.getHeader("Host"), string("www.baidu.com"));
  EXPECT_EQ(request.getHeader("User-Agent"), string(""));
  EXPECT_EQ(request.getHeader("Accept-Encoding"), string(""));
}