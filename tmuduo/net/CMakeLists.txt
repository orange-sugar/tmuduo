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

# install(TARGETS tmuduo_net DESTINATION lib)
# file(GLOB HEADERS "*.h")
# install(FILES ${HEADERS} DESTINATION include/tmuduo/net)