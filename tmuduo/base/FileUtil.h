#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "tmuduo/base/noncopyable.h"
#include "tmuduo/base/StringPiece.h"
#include <sys/types.h>

namespace tmuduo
{
namespace FileUtil {

class ReadSmallFile : noncopyable
{
  public:
    ReadSmallFile(StringArg filename);
    ~ReadSmallFile();

    // 返回errno
    template<typename String>
    int readToString(int maxSize,
                     String* content,
                     int64_t* fileSize,
                     int64_t* modifyTime,
                     int64_t* createTime);
    
    int readToBuffer(int* size);

    const char* buffer() const { return buf_; }    

    static const int kBufferSize = 64 * 1024;

  private:
    int fd_;
    int err_;
    char buf_[kBufferSize];
};

template<typename String>
int readFile(StringArg filename,
             int maxSize,
             String* content,
             int64_t* fileSize = nullptr,
             int64_t* modifyTime = nullptr,
             int64_t* createTime = nullptr)
{
  ReadSmallFile file(filename);
  return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

class AppendFile : noncopyable
{
  public:
    explicit AppendFile(StringArg filename);

    ~AppendFile();

    void append(const char* longline, const size_t len);

    void flush();
    
    off_t writtenBytes() const { return writtenBytes_; }
  
  private:
    size_t write(const char* longline, size_t len);
    FILE* fp_;
    char buffer_[64 * 1024];
    off_t writtenBytes_;
};

char* formattedName(const char* path);

} // namespace FileUtil
} // namespace tmuduo

#endif //  FILEUTIL_H