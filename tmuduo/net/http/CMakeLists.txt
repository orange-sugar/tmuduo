file(GLOB_RECURSE http_SRCS *.cc)

add_library(tmuduo_http ${http_SRCS})
target_link_libraries(tmuduo_http tmuduo_net)

install(TARGETS tmuduo_http DESTINATION lib)
set(HEADERS
  HttpContext.h
  HttpRequest.h
  HttpResponse.h
  HttpServer.h
  )
install(FILES ${HEADERS} DESTINATION include/tmuduo/net/http)