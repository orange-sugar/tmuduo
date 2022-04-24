# add_executable(test Buffer_tests.cc)
# target_link_libraries(test tmuduo_base tmuduo_net gtest gtest_main pthread)

add_executable(test test10.cc)
target_link_libraries(test tmuduo_base tmuduo_net)


# include(GoogleTest)
# gtest_discover_tests(test)

# add_executable(test7 test7.cc)
# target_link_libraries(test7 tmuduo_base tmuduo_net)

# add_executable(atomic_test atomicTest.cc)
# target_link_libraries(atomic_test tmuduo_base tmuduo_net)