file(GLOB_RECURSE base_srcs *.cc)
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

add_library(tmuduo_base ${base_srcs})
target_link_libraries(tmuduo_base pthread rt)

# install(TARGETS tmuduo_base DESTINATION lib)
# file(GLOB HEADERS "*.h")
# install(FILES ${HEADERS} DESTINATION include/tmuduo/base)

# if(NOT CMAKE_BUILD_NO_EXAMPLES)
#   add_subdirectory(tests)
# endif()