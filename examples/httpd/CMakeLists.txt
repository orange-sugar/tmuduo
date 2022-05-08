add_executable(httpserver httpserverTest.cc)
# target_include_directories(../tmuduo/net ../tmuduo/base)
target_link_libraries(httpserver tmuduo_net mysqlclient)

# find_package(MYSQL)
# if (MYSQL_FOUND)
#   message(STATUS "found mysql")
#   include_directories(${MYSQL_INCLUDE_DIR})
#   target_link_libraries(httpserver ${MYSQL_LIB})
# endif()

