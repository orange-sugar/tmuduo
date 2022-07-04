#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <sys/types.h>

#include <vector>

#include "tmuduo/base/StringPiece.h"
#include "tmuduo/base/Timestamp.h"
#include "tmuduo/base/Types.h"

namespace tmuduo
{
namespace ProcessInfo
{
  pid_t pid();
  std::string pidString();
  uid_t uid();
  std::string username();
  uid_t euid();
  Timestamp startTime();
  int clockTicksPerSecond();
  int pageSize();
  bool isDebugBuild();  // constexpr

  std::string hostname();
  std::string procname();
  StringPiece procname(const std::string& stat);

  /// read /proc/self/status
  std::string procStatus();

  /// read /proc/self/stat
  std::string procStat();

  /// read /proc/self/task/tid/stat
  std::string threadStat();

  /// readlink /proc/self/exe
  std::string exePath();

  int openedFiles();
  int maxOpenFiles();

  struct CpuTime
  {
    double userSeconds;
    double systemSeconds;

    CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }
  };
  CpuTime cpuTime();

  int numThreads();
  std::vector<pid_t> threads();
}
} // namespace tmuduo

#endif //  PROCESSINFO_H