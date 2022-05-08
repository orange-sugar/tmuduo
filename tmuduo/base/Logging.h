#ifndef LOGGING_H
#define LOGGING_H

// 利用LogStream、TimeStamp实现日志类

// TODO 利用可变模板，更改为printf(fmt, ...)风格

#include "tmuduo/base/LogStream.h"
#include "tmuduo/base/Timestamp.h"


#include "tmuduo/base/CurrentThread.h"

namespace tmuduo
{

class Logger
{
  public:
    enum LogLevel
    {
      TRACE,
      DEBUG,
      INFO,
      WARN,
      ERROR,
      FATAL,
      NUM_LOG_LEVELS,
    };

    class SourceFile
    {
      public:
        template<int N>
        SourceFile(const char (&arr)[N]) : data_(arr), size_(N-1)
        {
          const char* slash = strrchr(data_, '/');
          if (slash)
          {
            data_ = slash + 1;
            size_ -= static_cast<int>(data_ - arr);
          }
        }

        explicit SourceFile(const char* filename)
          : data_(filename)
        {
          const char* slash = strrchr(filename, '/');
          if (slash)
          {
            data_ = slash + 1;
          }
          size_ = static_cast<int>(strlen(data_));
        }

        const char* data_;
        int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);

    Logger(SourceFile file, int line, LogLevel level, const char* func, std::string backtrace);

    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    using OutputFunc = void (*)(const char* msg, int len);
    using FlushFunc = void (*)();
    static void setOutPut(OutputFunc);
    static void setFlush(FlushFunc);
    // TODO
    // static void setTimeZone(const TimeZone& tz); 
  
  private:
    class Impl 
    {
      public:
        using  LogLevel = Logger::LogLevel;
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
        void formatTime();
        void finish();

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    };

    Impl impl_;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
  return g_logLevel;
}

#define LOG_TRACE if (Logger::logLevel() <= Logger::TRACE) \
  Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (Logger::logLevel() <= Logger::DEBUG) \
  Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (Logger::logLevel() <= Logger::INFO) \
  Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#define LOG_SYSERR Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

#define CHECK_NOTNULL(val) \
  ::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

template <typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr)
{
  if (ptr == NULL)
  {
   Logger(file, line, Logger::FATAL).stream() << names;
  }
  return ptr;
}

} // namespace tmuduo

#endif