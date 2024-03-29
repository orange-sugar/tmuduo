#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include "tmuduo/base/ThreadPool.h"
#include "tmuduo/base/Timestamp.h"

using namespace tmuduo;
// int cnt = 0;
// std::mutex mutex_;
std::atomic<int> cnt = 0;

void threadFunc(int numCouts) {
  // {
  //   std::lock_guard<std::mutex> lock(mutex_);
  //   for (int i = 0; i < numCouts; ++i)
  //   {
  //     ++cnt;
  //   }
  // }
  for (int i = 0; i < numCouts; ++i) {
    ++cnt;
  }
}

int main(int argc, char* argv[]) {
  Timestamp start(Timestamp::now());
  ThreadPool pool(2);
  for (int j = 0; j < 2; ++j) {
    pool.pushTask(std::bind(threadFunc, atoi(argv[1])));
  }
  pool.waitForTasks();
  Timestamp end(Timestamp::now());
  std::cout << "Total time used: " << timeDifference(end, start) << "s\n";
  std::cout << "cnt = " << cnt << "\n";
}