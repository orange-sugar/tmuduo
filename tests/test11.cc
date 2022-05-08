#include <thread>

#include "tmuduo/net/TcpClient.h"

#include "tmuduo/base/Logging.h"
#include "tmuduo/net/EventLoop.h"
#include "tmuduo/net/InetAddress.h"

using namespace tmuduo;
using namespace tmuduo::net;

// class testClient
// {
//   public:
//   private:
//     EventLoop* loop_;
//     TcpClient client_;
// };

void threadFunc(EventLoop* loop)
{
  InetAddress serverAddr("127.0.0.1", 6912);
  TcpClient client(loop, serverAddr, "OrangeClient");
  client.connect();

  CurrentThread::sleepUSec(1000 * 1000);
}

int main()
{
  Logger::setLogLevel(Logger::DEBUG);

  EventLoop loop;
  loop.runAfter(3.0, std::bind(&EventLoop::quit, &loop));
  std::thread t1(std::bind(threadFunc, &loop));
  loop.loop();
  t1.join();
}