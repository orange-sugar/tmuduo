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

# if (VERRBOSE_FLAG EQUAL ON)
  message("net_srcs ${net_srcs}")
# endif()

add_library(tmuduo_net ${net_srcs})
target_link_libraries(tmuduo_net tmuduo_base mysqlclient)

add_subdirectory(http)
add_subdirectory(sql)

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
  sql/sqlConnectionPool.h
  )
install(FILES ${HEADERS} DESTINATION include/tmuduo/net)
