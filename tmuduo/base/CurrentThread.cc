#include "tmuduo/base/CurrentThread.h"

#include <cxxabi.h>
#include <execinfo.h>
#include <cstdlib>

namespace tmuduo
{
namespace CurrentThread {

pid_t gettid()
{
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "unknown";

static_assert(std::is_same<int, pid_t>::value, "pid should be int");

// std::string myStackTrace(bool demangle)
// { 
//   std::string stack;
//   const int max_frames = 200;
//   void* addrList[max_frames];

//   int addrLen = ::backtrace(addrList, sizeof(addrList)/sizeof(void*));

//   if (addrLen == 0) return stack;

//   char** symbolList = ::backtrace_symbols(addrList, addrLen);

//   size_t funcNameSize = 256;
//   char* funcName = static_cast<char*>(::malloc(funcNameSize));

//   for (int i = 1; i < addrLen; ++i)
//   {
//     char* beginName = 0, *beginOffset = 0, *endOffset = 0;

//     for (char* p = symbolList[i]; *p; ++p)
//     {
//       if (*p == '(')
//       {
//         beginName = p;
//       }
//       else if (*p == '+')
//       {
//         beginOffset = p;
//       }
//       else if (*p == ')')
//       {
//         endOffset = p;
//         break;
//       }
//     }

//     if (beginName && beginOffset && endOffset && beginName < beginOffset)
//     {
//       *beginName++ = '\0';
//       *beginOffset++ = '\0';
//       *endOffset++ = '\0';

//       int status;
//       char* ret = ::abi::__cxa_demangle(beginName, funcName, &funcNameSize, &status);
//       if (status == 0)
//       {
//         funcName = ret;
//         stack += "  ";
//         stack.append(symbolList[i]);
//         stack.append(" : ");
//         stack.append(funcName);
//         stack.append("+");
//         stack.append(beginOffset);
//         stack.append("\n");
//       }
//       else   
//       {
//         stack += "  ";
//         stack.append(symbolList[i]);
//         stack.append(" : ");
//         stack.append(beginName);
//         stack.append("+");
//         stack.append(beginOffset);
//         stack.append("\n");
//       }
//     }
//     else  
//     {
//       stack.append("  ");
//       stack.append(symbolList[i]);
//       stack.append("\n");
//     }
//   }
//   return stack;
// }

std::string stackTrace(bool demangle)
{
  std::string stack;
  const int max_frames = 200;
  void* frame[max_frames];
  int nptrs = ::backtrace(frame, max_frames);
  char** strings = ::backtrace_symbols(frame, nptrs);
  if (strings)
  {
    size_t len = 256;
    char* demangled = demangle ? static_cast<char*>(::malloc(len)) : nullptr;
    for (int i = 1; i < nptrs; ++i)  // skipping the 0-th, which is this function
    {
      if (demangle)
      {
        // https://panthema.net/2008/0901-stacktrace-demangled/
        // bin/exception_test(_ZN3Bar4testEv+0x79) [0x401909]
        char* left_par = nullptr;
        char* plus = nullptr;
        for (char* p = strings[i]; *p; ++p)
        {
          if (*p == '(')
            left_par = p;
          else if (*p == '+')
            plus = p;
        }

        if (left_par && plus)
        {
          *plus = '\0';
          int status = 0;
          char* ret = abi::__cxa_demangle(left_par+1, demangled, &len, &status);
          *plus = '+';
          if (status == 0)
          {
            demangled = ret;  // ret could be realloc()
            stack.append(strings[i], left_par+1);
            stack.append(demangled);
            stack.append(plus);
            stack.push_back('\n');
            continue;
          }
        }
      }
      // Fallback to mangled names
      stack.append(strings[i]);
      stack.push_back('\n');
    }
    free(demangled);
    free(strings);
  }
  return stack;
}

bool isMainThread()
{
  return tid() == ::getpid();
}

void sleepUSec(int64_t usec)
{
  struct timespec ts = {0, 0};
  ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(usec & Timestamp::kMicroSecondsPerSecond * 1000);
  ::nanosleep(&ts, nullptr);
}

void cacheTid()
{
  if (t_cachedTid == 0)
  {
    t_cachedTid = gettid();
    t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%6d ", t_cachedTid);
    // printf("%d", t_tidStringLength);
  }
}

} // namespace CurrentThread
} // namespace tmuduo