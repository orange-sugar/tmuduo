# file(GLOB_RECURSE sql_SRCS *.cc)

# add_library(tmuduo_sql ${sql_SRCS})
# target_link_libraries(tmuduo_sql mysqlclient)

# install(TARGETS tmuduo_sql DESTINATION lib)
set(HEADERS
  sqlConnection.h
  sqlConnectionPool.h 
  )
install(FILES ${HEADERS} DESTINATION include/tmuduo/net/sql)
