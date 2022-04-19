## s01
实现EvenLoop封装，poll一段时间后自动退出。

## s02
[2022.04.10]实现Channel类、Poller类，EventLoop循环可以调用Poller接口监听事件。每个Channel类关心一个文件描述符。

## s03
[2022.04.15]实现TimerQUeue定时器，完成定时与取消任务。

## s04
[2022.04.15]实现EventLoop::runInLoop(), 可以跨线程调度任务。
[2022.04.16]修改TimerQueue::addTimer、TimerQueue::addTimerInLoop, ~~解决unique_ptr传递问题。~~

## s05
[2022.04.17]封装EventLoopThread类，同时修复了TimerQueue::addTimer()中lambda表达式的传值问题。

## s06
[2022.04.19]封装套接字的创建、设置、侦听、绑定、接受等操作；封装ip地址。

## s07
[2022.04.19]完成Acceptor，封装了监听、接受操作。当IO loop接收到连接请求时，调用接收器的回调函数完成accept操作。

