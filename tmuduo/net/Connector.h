#ifndef TMUDUO_NET_CONNECTOR_H
#define TMUDUO_NET_CONNECTOR_H

#include <atomic>
#include <functional>
#include <memory>

#include "tmuduo/base/Logging.h"
#include "tmuduo/net/InetAddress.h"

namespace tmuduo
{
namespace net
{

class Channel;
class EventLoop;

class Connector : noncopyable,
                  public std::enable_shared_from_this<Connector>
{
  public:
    using NewConnectionCalback = std::function<void(int sockfd)>;

    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(NewConnectionCalback cb)
    { newConnectionCalback_ = std::move(cb); }

    void start();
    void restart();
    void stop();

    const InetAddress& serverAddress() const { return serverAddr_; }

  private:
    enum class States {kDisconnected, kConnecting, kConnected};
    static const int kMaxRetryDelayMs = 30*1000;
    static const int kInitRetryDelayMs = 500;

    void setState(States s) { state_.store(s); }

    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

    std::string_view stateToString(States s);

    EventLoop* loop_;
    InetAddress serverAddr_;
    std::atomic<bool> connect_;
    std::atomic<States> state_;
    std::unique_ptr<Channel> channel_;
    NewConnectionCalback newConnectionCalback_;
    int retryDelayMs_;
};

} //  namespace net
} //  namespace tmuduo

#endif	// TMUDUO_NET_CONNECTOR_H