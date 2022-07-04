#ifndef TMUDUO_BASE_ASYNCLOGGING_H
#define TMUDUO_BASE_ASYNCLOGGING_H

#include "tmuduo/base/BlockingQueue.h"
#include "tmuduo/base/BoundedBlockingQueue.h"
#include "tmuduo/base/CountDownLatch.h"
#include "tmuduo/base/LogStream.h"

#include "tmuduo/base/Logging.h"

#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <functional>

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
      // thread_ = std::unique_ptr<std::thread>(
      //   new std::thread(std::bind(&AsyncLogging::threadFunc, this))
      // );
      thread_.reset(new std::thread(std::bind(&AsyncLogging::threadFunc, this)));
      latch_.wait();
    }

    void stop()
    {
      if (running_)
      {
        running_ = false;
        cond_.notify_one();
        // LOG_DEBUG << " " << thread_->joinable();
        if (thread_->joinable())
        {
          // LOG_DEBUG << " ";
          thread_->join();
        }
      }
    }

  private:
    void threadFunc();

    using Buffer = detail::FixedBuffer<detail::kLargeBuffer>;
    using BufferVector = std::vector<std::unique_ptr<Buffer>>;
    using BufferPtr = BufferVector::value_type;
    
    const int flushInterval_;
    std::atomic<bool> running_;
    const std::string basename_;
    const off_t rollSize_;
    std::unique_ptr<std::thread> thread_;
    
    CountDownLatch latch_;
    std::mutex mutex_;
    std::condition_variable cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;

};

} // namespace tmuduo

#endif	// TMUDUO_BASE_ASYNCLOGGING_H