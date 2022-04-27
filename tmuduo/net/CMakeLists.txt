file(GLOB_RECURSE net_srcs *.cc)
# set(base_SRCS
#   ${base_srcs}
#   # AsyncLogging.cc
#   # BlockingQueue.cc
#   # BoundedBlockingQueue.cc
#   # CountDownLatch.cc
#   # CurrentThread.cc 
#   # FileUtil.cc 
#   # LogFile.cc 
#   # Logging.cc 
# )

add_library(tmuduo_net ${net_srcs})
target_link_libraries(tmuduo_net tmuduo_base)

add_subdirectory(http)

install(TARGETS tmuduo_net DESTINATION lib)

set(HEADERS
  Buffer.h
  Callbacks.h
  Channel.h
  Endian.h
  EventLoop.h
  EventLoopThread.h
  EventLoopThreadPool.h
  InetAddress.h
  TcpClient.h
  TcpConnection.h
  TcpServer.h
  TimerId.h
  )
install(FILES ${HEADERS} DESTINATION include/tmuduo/net)