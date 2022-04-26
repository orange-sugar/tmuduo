file(GLOB_RECURSE http_SRCS *.cc)

add_library(tmuduo_http ${http_SRCS})
target_link_libraries(tmuduo_http tmuduo_net)