file(GLOB_RECURSE base_srcs *.cc)

add_library(tmuduo_base ${base_srcs})
target_link_libraries(tmuduo_base pthread rt)

install(TARGETS tmuduo_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/tmuduo/base)
