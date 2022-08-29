cmake_minimum_required(VERSION 2.6)
set (CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(tmuduo C CXX)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

option(TMUDUO_BUILD_TESTS "Build tests" on)

set(CXX_FLAGS
  -g
  -D_FILE_OFFSET_BITS=64
  -Wall
  -Wextra
  -Werror
  -Wconversion
  -Wno-unused-parameter
  -Wold-style-cast
  -Woverloaded-virtual
  -Wpointer-arith
  -Wshadow
  -Wwrite-strings
  -march=native
 # -MMD
  -std=c++17
  -rdynamic
  -fstandalone-debug
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(APPEND CXX_FLAGS "-Wno-null-dereference")
  list(APPEND CXX_FLAGS "-Wno-sign-conversion")
  list(APPEND CXX_FLAGS "-Wno-unused-local-typedef")
  list(APPEND CXX_FLAGS "-Wthread-safety")
  list(REMOVE_ITEM CXX_FLAGS "-rdynamic")
endif()
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
# message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

add_subdirectory(tmuduo/base)
add_subdirectory(tmuduo/net)
add_subdirectory(tests)
add_subdirectory(examples)

# clang-format
if (NOT DEFINED CLANG_FORMAT_BIN) 
  find_program(CLANG_FORMAT_BIN
    NAMES clang-format clang-format-16
    HINTS ${PROJECT_SOURCE_DIR})
endif()
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
  message(WARNING "couldn't find clang-format.")
else()
  message(STATUS "found clang-format at ${CLANG_FORMAT_BIN}")
endif()

# make format
string(CONCAT FORMAT_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/tmuduo,"
  # "${CMAKE_CURRENT_SOURCE_DIR}/examples,"
  # "${CMAKE_CURRENT_SOURCE_DIR}/tests,"
  )

add_custom_target(format ${CMAKE_SOURCE_DIR}/run_clang_format.py
  ${CLANG_FORMAT_BIN}
  ${CMAKE_CURRENT_SOURCE_DIR}/clang_format_exclusions.txt
  --source_dirs
  ${FORMAT_DIRS}
  --fix
  # --quiet
  )

# enable_testing()
