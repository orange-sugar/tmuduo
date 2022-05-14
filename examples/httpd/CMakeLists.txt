add_executable(webserver main.cc WebServer.cc)
target_link_libraries(webserver tmuduo_net mysqlclient)

# find_package(MYSQL)
# if (MYSQL_FOUND)
#   message(STATUS "found mysql")
#   include_directories(${MYSQL_INCLUDE_DIR})
#   target_link_libraries(httpserver ${MYSQL_LIB})
# endif()

# add_executable(pooltest poolTest.cc sqlConnectionPool.cc)
# target_link_libraries(pooltest tmuduo_base tmuduo_net mysqlclient)

