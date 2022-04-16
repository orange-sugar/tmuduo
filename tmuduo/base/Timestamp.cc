#include "tmuduo/base/Timestamp.h"
#include <sys/time.h>
#include <stdio.h>
#include <inttypes.h>


static_assert(sizeof(Timestamp) == sizeof(int64_t),
              "Timestamp is same size as int64_t");

std::string Timestamp::toString() const
{
  char buf[32] = {0};
  int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
  // stupid way to get time in East Eight Erea
  seconds += 8 * 60 * 60;
  int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
  return buf;
}

Timestamp Timestamp::now()
{
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  int64_t seconds = tv.tv_sec;
  return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

std::string Timestamp::toFormattedString(bool showMicroseconds) const
{
  char buf[64] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  if (showMicroseconds)
  {
    int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
    // 默认北京时间，东八区
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
           tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
           tm_time.tm_hour + 8, tm_time.tm_min, tm_time.tm_sec, 
           microseconds);
  }
  else {
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
           tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
           tm_time.tm_hour + 8, tm_time.tm_min, tm_time.tm_sec 
           );
  }
  return buf;
}

// #include <iostream>
// int main()
// {
  
//   Timestamp ts(Timestamp::now());
//   auto time_ = ts.toFormattedString(false);
//   std::cout << time_ << "\n";
// }