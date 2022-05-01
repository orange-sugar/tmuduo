# add_executable(test HttpRequest_unittest.cc)
# target_link_libraries(test tmuduo_base tmuduo_net gtest gtest_main)

add_executable(server TestServer.cc)
target_link_libraries(server tmuduo_base tmuduo_net)

# add_executable(client test11.cc)
# target_link_libraries(client tmuduo_base tmuduo_net)

# add_executable(test HttpServerTest.cc)
# target_link_libraries(test 
# tmuduo_base tmuduo_net
# )

# include(GoogleTest)
# gtest_discover_tests(test)

# add_executable(test7 test7.cc)
# target_link_libraries(test7 tmuduo_base tmuduo_net)

# add_executable(atomic_test atomicTest.cc)
# target_link_libraries(atomic_test tmuduo_base tmuduo_net)
