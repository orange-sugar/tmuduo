#ifndef EXAMPLES_CHAT_CODEC_H
#define EXAMPLES_CHAT_CODEC_H

#include "tmuduo/base/Logging.h"
#include "tmuduo/net/Buffer.h"
#include "tmuduo/net/Endian.h"
#include "tmuduo/net/TcpConnection.h"

class LengthHeaderCodec : noncopyable
{
  public:
    using StringMessageCallback = 
      std::function<void(const tmuduo::net::TcpConnectionPtr&,
                         const std::string& message,
                         Timestamp)>;
      explicit LengthHeaderCodec(StringMessageCallback cb)
        : messageCallback_(std::move(cb))
      { }

    void onMessage(const tmuduo::net::TcpConnectionPtr& conn,
                   tmuduo::net::Buffer* buf,
                   Timestamp receiveTime)
    {
      while (buf->readableBytes() >= kHeaderLen) {
        const void* data = buf->peek();
        int32_t be32 = *static_cast<const int32_t*>(data);
        const int32_t len = tmuduo::net::sockets::networkToHost32(be32);
        if (len > 65536 || len < 0)
        {
          LOG_SYSERR << "Invalid length" << len;
          conn->shutdown();
          break;
        }
        else if (buf->readableBytes() >= len + kHeaderLen)
        {
          buf->retrieve(kHeaderLen);
          std::string message(buf->peek(), len);
          messageCallback_(conn, message, receiveTime);
          buf->retrieve(len);
        }
        else  
        {
          break;
        }
      }
    } 

    void send(tmuduo::net::TcpConnection* conn, const std::string& message)
    {
      tmuduo::net::Buffer buf;
      buf.append(message.data(), message.size());
      int32_t len = static_cast<int32_t>(message.size());
      int32_t be32 = tmuduo::net::sockets::hostToNetwork32(len);
      buf.prepend(&be32, sizeof(be32));
    }

  private:
    StringMessageCallback messageCallback_;
    const static size_t kHeaderLen = sizeof(int32_t);
};

#endif	// EXAMPLES_CHAT_CODEC_H