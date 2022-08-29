#include "tmuduo/base/BoundedBlockingQueue.h"
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <vector>
#include "tmuduo/base/CountDownLatch.h"
#include "tmuduo/base/CurrentThread.h"

using namespace tmuduo;

class Test {
 public:
 private:
  void threadFunc() {
    // TODO states of std::thread?
    printf("td=%d started\n", CurrentThread::tid());
    latch_.countDown();
    bool running = true;
    while (running) {
      std::thread d(queue_.take());
    }
  }

  BoundedBlockingQueue<std::string> queue_;
  CountDownLatch latch_;
  std::vector<std::unique_ptr<std::thread>> threads_;
};