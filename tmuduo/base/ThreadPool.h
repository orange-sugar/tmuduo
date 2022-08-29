#ifndef THREADPOOL_H
#define THREADPOOL_H

// https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>
#include "tmuduo/base/Types.h"

namespace tmuduo {

class ThreadPool {
  using ui32 = std::uint_fast32_t;
  using ui64 = std::uint_fast64_t;

 public:
  ThreadPool(const ui32& threadNum = std::thread::hardware_concurrency())
      : threadNum_(threadNum), threads_(new std::thread[threadNum]) {
    createThreads();
  }

  ~ThreadPool() {
    waitForTasks();
    running_ = false;
    destroyThreads();
  }

  ui64 getTasksQueued() const {
    const std::scoped_lock lock(mutex_);
    return tasks_.size();
  }

  ui32 getTasksRunning() const {
    return tasksTotal_ - static_cast<ui32>(getTasksQueued());
  }

  ui32 getTasksTotal() const {
    return tasksTotal_;
  }

  ui32 getThreadCount() const {
    return threadNum_;
  }

  void waitForTasks() {
    while (true) {
      if (!paused_) {
        if (tasksTotal_ == 0) {
          break;
        }
      } else {
        if (getTasksRunning() == 0) {
          break;
        }
      }
      sleepOrYield();
    }
  }

  // template<typename T1, typename T2, typename F>
  // void parallelizeLoop(const T1& firstIndex,
  //                      const T2& indexAfterLast,
  //                      const F& loop, ui32 blocksNum = 0)
  // {

  // }

  template <typename F>
  void pushTask(const F& task) {
    tasksTotal_++;
    {
      const std::scoped_lock lock(mutex_);
      tasks_.push(std::function<void()>(task));
    }
  }

  template <typename F, typename... A>
  void pushTask(const F& task, const A&... args) {
    pushTask([task, args...] { task(args...); });
  }

  void reset(const ui32& threadNum = std::thread::hardware_concurrency()) {
    bool wasPaused = paused_;
    waitForTasks();
    running_ = false;
    destroyThreads();
    threadNum_ = threadNum ? threadNum : std::thread::hardware_concurrency();
    threads_.reset(new std::thread[threadNum_]);
    paused_ = wasPaused;
    running_ = true;
    createThreads();
  }

  template <typename F, typename... A,
            typename = std::enable_if_t<
                std::is_void_v<std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>>>
  std::future<bool> submit(const F& task, const A&... args) {
    std::shared_ptr<std::promise<bool>> taskPromise(new std::promise<bool>);
    std::future<bool> future = taskPromise->get_future();
    push_back([task, args..., taskPromise] {
      try {
        task(args...);
        taskPromise->set_value(true);
      } catch (...) {
        try {
          taskPromise->set_exception(std::current_exception());
        } catch (...) {
        }
      }
    });
    return future;
  }

  template <typename F, typename... A,
            typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>,
            typename = std::enable_if_t<!std::is_void_v<R>>>
  std::future<R> submit(const F& task, const A&... args) {
    std::shared_ptr<std::promise<R>> taskPromise(new std::promise<R>);
    std::future<R> future = taskPromise->get_future();
    push_task([task, args..., taskPromise] {
      try {
        taskPromise->set_value(task(args...));
      } catch (...) {
        try {
          taskPromise->set_exception(std::current_exception());
        } catch (...) {
        }
      }
    });
    return future;
  }

  std::atomic<bool> paused_ = false;
  ui32 sleep_duration = 1000;

 private:
  void createThreads() {
    for (ui32 i = 0; i < threadNum_; ++i) {
      threads_[i] = std::thread(&ThreadPool::worker, this);
    }
  }

  void destroyThreads() {
    for (ui32 i = 0; i < threadNum_; ++i) {
      threads_[i].join();
    }
  }

  bool popTask(std::function<void()>& task) {
    const std::scoped_lock lock(mutex_);
    if (tasks_.empty()) {
      return false;
    } else {
      task = std::move(tasks_.front());
      tasks_.pop();
      return true;
    }
  }

  void sleepOrYield() {
    if (sleep_duration) std::this_thread::sleep_for(std::chrono::microseconds(sleep_duration));
    else
      std::this_thread::yield();
  }

  void worker() {
    while (running_) {
      std::function<void()> task;
      if (!paused_ && popTask(task)) {
        task();
        tasksTotal_--;
      } else {
        sleepOrYield();
      }
    }
  }

  mutable std::mutex mutex_ = {};
  std::atomic<bool> running_ = true;
  std::queue<std::function<void()>> tasks_ = {};
  ui32 threadNum_;
  std::unique_ptr<std::thread[]> threads_;
  std::atomic<ui32> tasksTotal_ = 0;
};

}  // namespace tmuduo

#endif