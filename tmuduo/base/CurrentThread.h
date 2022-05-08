#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include "tmuduo/base/Types.h"
#include "tmuduo/base/Timestamp.h"

#include <cstdio>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace tmuduo
{
namespace CurrentThread 
{
  pid_t gettid();
  
  // internel
  extern __thread int t_cachedTid;
  extern __thread char t_tidString[32];
  extern __thread int t_tidStringLength;
  extern __thread const char* t_threadName;
  // 缓存tid，提高访问速率，同时要注意检查调用者合法性
  void cacheTid();

  inline int tid()
  {
    if (__builtin_expect(t_cachedTid == 0, 0))
    {
      cacheTid();
    }
    return t_cachedTid;
  }
  
  inline const char* tidString()
  {
    return t_tidString;
  }

  inline int tidStringLength()
  {
    return t_tidStringLength;
  }

  inline const char* name()
  {
    return t_threadName;
  }

  bool isMainThread();

  void sleepUSec(int64_t usec);

  std::string stackTrace(bool demangle);

} // namespace CurrentThread
} // namespace tmuduo

#endif //  CURRENTTHREAD_H