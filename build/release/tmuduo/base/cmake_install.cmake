# Install script for directory: /home/orange/tMuduo/tmuduo/base

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/orange/tMuduo/build/release/lib/libtmuduo_base.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/tmuduo/base" TYPE FILE FILES
    "/home/orange/tMuduo/tmuduo/base/AsyncLogging.h"
    "/home/orange/tMuduo/tmuduo/base/BlockingQueue.h"
    "/home/orange/tMuduo/tmuduo/base/BoundedBlockingQueue.h"
    "/home/orange/tMuduo/tmuduo/base/CountDownLatch.h"
    "/home/orange/tMuduo/tmuduo/base/CurrentThread.h"
    "/home/orange/tMuduo/tmuduo/base/FileUtil.h"
    "/home/orange/tMuduo/tmuduo/base/LogFile.h"
    "/home/orange/tMuduo/tmuduo/base/LogStream.h"
    "/home/orange/tMuduo/tmuduo/base/Logging.h"
    "/home/orange/tMuduo/tmuduo/base/ProcessInfo.h"
    "/home/orange/tMuduo/tmuduo/base/StringPiece.h"
    "/home/orange/tMuduo/tmuduo/base/ThreadPool.h"
    "/home/orange/tMuduo/tmuduo/base/Timestamp.h"
    "/home/orange/tMuduo/tmuduo/base/Types.h"
    "/home/orange/tMuduo/tmuduo/base/copyable.h"
    "/home/orange/tMuduo/tmuduo/base/noncopyable.h"
    )
endif()

