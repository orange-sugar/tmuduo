file(GLOB_RECURSE sql_SRCS *.cc)

add_library(tmuduo_net ${sql_SRCS})
target_link_libraries(tmuduo_net mysqlclient)

install(TARGETS tmuduo_http DESTINATION lib)
set(HEADERS
  sqlConnection.h
  sqlConnectionPool.h 
  )
install(FILES ${HEADERS} DESTINATION include/tmuduo/net/sql)