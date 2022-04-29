# Install script for directory: /home/orange/tMuduo/tmuduo/net

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/orange/tMuduo/build/release/release")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/orange/tMuduo/build/release/lib/libtmuduo_net.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/tmuduo/net" TYPE FILE FILES
    "/home/orange/tMuduo/tmuduo/net/Buffer.h"
    "/home/orange/tMuduo/tmuduo/net/Callbacks.h"
    "/home/orange/tMuduo/tmuduo/net/Channel.h"
    "/home/orange/tMuduo/tmuduo/net/Endian.h"
    "/home/orange/tMuduo/tmuduo/net/EventLoop.h"
    "/home/orange/tMuduo/tmuduo/net/EventLoopThread.h"
    "/home/orange/tMuduo/tmuduo/net/EventLoopThreadPool.h"
    "/home/orange/tMuduo/tmuduo/net/InetAddress.h"
    "/home/orange/tMuduo/tmuduo/net/TcpClient.h"
    "/home/orange/tMuduo/tmuduo/net/TcpConnection.h"
    "/home/orange/tMuduo/tmuduo/net/TcpServer.h"
    "/home/orange/tMuduo/tmuduo/net/TimerId.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/orange/tMuduo/build/release/tmuduo/net/http/cmake_install.cmake")

endif()
