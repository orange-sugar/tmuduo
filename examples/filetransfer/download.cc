#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/TcpServer.h"

#include <stdio.h>
#include <unistd.h>

using namespace tmuduo::net;
using std::string;

const char* g_file = nullptr;

std::string readFile(const char* filename)
{
  string content;
  FILE* fp = ::fopen(filename, "rb");
  if (fp)
  {
    const int kBufSize = 1024 * 1024;
    char iobuf[kBufSize];
    // setbuffer: 打开文件流后，在读取内容之前设置文件流缓冲区
    // alias of setvbuf()
    ::setbuffer(fp, iobuf, sizeof(iobuf));

    char buf[kBufSize];
    size_t nread = 0;
    while ((nread = ::fread(buf, 1, sizeof(buf), fp)) > 0)
    {
      content.append(buf, nread);
    }
    ::fclose(fp);
  }
  return content;
}

void onHighWaterMark(const TcpConnectionPtr& conn, size_t len)
{
  LOG_INFO << "HighWaterMark " << len;
}

void onConnection(const TcpConnectionPtr& conn)
{
  LOG_INFO << "FileServer - " << conn->peerAddress().toIpPort() << " -> "
           << conn->localAddress().toIpPort() << " is "
           << (conn->connected() ? "UP" : "DOWN");
  if (conn->connected())
  {
    LOG_INFO << "FileServer - Sending file " << g_file << " to "
             << conn->peerAddress().toIpPort();
    conn->setHighWaterMarkCallback(onHighWaterMark, 64*1024);
    string fileContent = readFile(g_file);
    conn->send(fileContent);
    conn->shutdown();
    LOG_INFO << "FileServer - done";
  }
}

int main(int argc, char* argv[])
{
  LOG_INFO << "pid = " << getpid();
  if (argc > 1)
  {
    g_file = argv[1];

    EventLoop loop;
    InetAddress listenAddr(2022);
    TcpServer server(&loop, listenAddr, "FileServer", TcpServer::Option::kReusePort);
    server.setConnectionCallback(onConnection);
    server.start();
    loop.loop();
  }
  else  
  {
    fprintf(stderr, "Usage: %s file_for_downloading\n", argv[0]);
  }
}