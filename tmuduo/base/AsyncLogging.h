#ifndef ASYNCLOGGING_H
#define ASYNCLOGGING_H

#include "tmuduo/base/BlockingQueue.h"
#include "tmuduo/base/BoundedBlockingQueue.h"
#include "tmuduo/base/CountDownLatch.h"
#include "tmuduo/base/LogStream.h"

#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <functional>

using std::mutex;
using std::unique_ptr;
using std::thread;

namespace tmuduo
{

class AsyncLogging : noncopyable
{
  public:
    AsyncLogging(const std::string& basename, off_t rollSize, int flushInterval = 3);
    ~AsyncLogging()
    {
      if (running_)
      {
        stop();
      }
    }

    void append(const char* logline, int len);

    void start()
    {
      running_ = true;
      thread_ = unique_ptr<thread>(
        new thread(std::bind(&AsyncLogging::threadFunc, this))
      );
      latch_.wait();
    }

    void stop()
    {
      running_ = false;
      cond_.notify_one();
      thread_->join();
    }

  private:
    void threadFunc();

    using Buffer = detail::FixedBuffer<detail::kLargeBuffer>;
    using BufferVector = std::vector<unique_ptr<Buffer>>;
    using BufferPtr = BufferVector::value_type;
    
    const int flushInterval_;
    std::atomic<bool> running_;
    const std::string basename_;
    const off_t rollSize_;
    unique_ptr<thread> thread_;
    // std::function<void()> threadfunc_;
    CountDownLatch latch_;
    mutex mutex_;
    condition_variable cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;

};

} // namespace tmuduo

#endif