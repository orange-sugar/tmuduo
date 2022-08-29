#ifndef TMUDUO_NET_CHANNEL_H
#define TMUDUO_NET_CHANNEL_H

#include <functional>
#include <memory>
#include "tmuduo/base/Timestamp.h"
#include "tmuduo/base/noncopyable.h"

namespace tmuduo {
namespace net {
class EventLoop;

class Channel : noncopyable {
 public:
  using EventCallback = std::function<void()>;
  using ReadEventCallback = std::function<void(Timestamp)>;

  Channel(EventLoop* loop, int fd);
  ~Channel();

  void handleEvent(Timestamp receiveTime);
  void setReadCallback(ReadEventCallback cb) {
    readCallback_ = std::move(cb);
  }
  void setWriteCallback(EventCallback cb) {
    writeCallback_ = std::move(cb);
  }
  void setCloseCallback(EventCallback cb) {
    closeCallback_ = std::move(cb);
  }
  void setErrorCallback(EventCallback cb) {
    errorCallback_ = std::move(cb);
  }

  void tie(const std::shared_ptr<void>&);

  int fd() const {
    return fd_;
  }
  int events() const {
    return events_;
  }
  void setRevents(int revt) {
    revents_ = revt;
  }
  bool isNoneEvent() const {
    return events_ == kNoneEvent;
  }

  void enableReading() {
    events_ |= kReadEvent;
    update();
  }
  void disableReading() {
    events_ &= ~kReadEvent;
    update();
  }
  void enableWriting() {
    events_ |= kWriteEvent;
    update();
  }
  void disableWriting() {
    events_ &= ~kWriteEvent;
    update();
  }
  void disableAll() {
    events_ = kNoneEvent;
    update();
  }

  bool isWriting() const {
    return events_ & kWriteEvent;
  }
  bool isReading() const {
    return events_ & kReadEvent;
  }

  // Poller使用
  int index() {
    return index_;
  }
  void setIndex(int idx) {
    index_ = idx;
  }

  // debug
  std::string reventsToString() const;
  std::string eventsToString() const;

  void doNotLogHup() {
    logHup_ = false;
  }

  EventLoop* ownerLoop() const {
    return loop_;
  }

  void remove();

 private:
  static std::string eventsToString(int fd, int ev);
  void update();
  void handleEventWithguard(Timestamp receiveTime);

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  EventLoop* loop_;  // 所属EventLoop
  const int fd_;     // 不负责关闭
  int events_;
  int revents_;
  int index_;    // 在Poller中的序号
  bool logHup_;  // POLLHUP

  std::weak_ptr<void> tie_;
  bool tied_;
  bool eventHandling_;
  bool addedToLoop_;
  ReadEventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback closeCallback_;
  EventCallback errorCallback_;
};

}  // namespace net
}  //  namespace tmuduo

#endif  // TMUDUO_NET_CHANNEL_H