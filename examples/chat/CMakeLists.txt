add_executable(chat_server chat_server.cc)
target_link_libraries(chat_server tmuduo_net)

add_executable(chat_client chat_client.cc)
target_link_libraries(chat_client tmuduo_net)