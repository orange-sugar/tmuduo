# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/orange/tMuduo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/orange/tMuduo/build/release

# Include any dependencies generated for this target.
include tmuduo/net/http/CMakeFiles/tmuduo_http.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tmuduo/net/http/CMakeFiles/tmuduo_http.dir/compiler_depend.make

# Include the progress variables for this target.
include tmuduo/net/http/CMakeFiles/tmuduo_http.dir/progress.make

# Include the compile flags for this target's objects.
include tmuduo/net/http/CMakeFiles/tmuduo_http.dir/flags.make

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpContext.cc.o: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/flags.make
tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpContext.cc.o: ../../tmuduo/net/http/HttpContext.cc
tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpContext.cc.o: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/orange/tMuduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpContext.cc.o"
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpContext.cc.o -MF CMakeFiles/tmuduo_http.dir/HttpContext.cc.o.d -o CMakeFiles/tmuduo_http.dir/HttpContext.cc.o -c /home/orange/tMuduo/tmuduo/net/http/HttpContext.cc

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpContext.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tmuduo_http.dir/HttpContext.cc.i"
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/orange/tMuduo/tmuduo/net/http/HttpContext.cc > CMakeFiles/tmuduo_http.dir/HttpContext.cc.i

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpContext.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tmuduo_http.dir/HttpContext.cc.s"
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/orange/tMuduo/tmuduo/net/http/HttpContext.cc -o CMakeFiles/tmuduo_http.dir/HttpContext.cc.s

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpResponse.cc.o: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/flags.make
tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpResponse.cc.o: ../../tmuduo/net/http/HttpResponse.cc
tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpResponse.cc.o: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/orange/tMuduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpResponse.cc.o"
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpResponse.cc.o -MF CMakeFiles/tmuduo_http.dir/HttpResponse.cc.o.d -o CMakeFiles/tmuduo_http.dir/HttpResponse.cc.o -c /home/orange/tMuduo/tmuduo/net/http/HttpResponse.cc

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpResponse.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tmuduo_http.dir/HttpResponse.cc.i"
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/orange/tMuduo/tmuduo/net/http/HttpResponse.cc > CMakeFiles/tmuduo_http.dir/HttpResponse.cc.i

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpResponse.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tmuduo_http.dir/HttpResponse.cc.s"
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/orange/tMuduo/tmuduo/net/http/HttpResponse.cc -o CMakeFiles/tmuduo_http.dir/HttpResponse.cc.s

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpServer.cc.o: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/flags.make
tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpServer.cc.o: ../../tmuduo/net/http/HttpServer.cc
tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpServer.cc.o: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/orange/tMuduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpServer.cc.o"
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpServer.cc.o -MF CMakeFiles/tmuduo_http.dir/HttpServer.cc.o.d -o CMakeFiles/tmuduo_http.dir/HttpServer.cc.o -c /home/orange/tMuduo/tmuduo/net/http/HttpServer.cc

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpServer.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tmuduo_http.dir/HttpServer.cc.i"
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/orange/tMuduo/tmuduo/net/http/HttpServer.cc > CMakeFiles/tmuduo_http.dir/HttpServer.cc.i

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpServer.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tmuduo_http.dir/HttpServer.cc.s"
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/orange/tMuduo/tmuduo/net/http/HttpServer.cc -o CMakeFiles/tmuduo_http.dir/HttpServer.cc.s

# Object files for target tmuduo_http
tmuduo_http_OBJECTS = \
"CMakeFiles/tmuduo_http.dir/HttpContext.cc.o" \
"CMakeFiles/tmuduo_http.dir/HttpResponse.cc.o" \
"CMakeFiles/tmuduo_http.dir/HttpServer.cc.o"

# External object files for target tmuduo_http
tmuduo_http_EXTERNAL_OBJECTS =

lib/libtmuduo_http.a: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpContext.cc.o
lib/libtmuduo_http.a: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpResponse.cc.o
lib/libtmuduo_http.a: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/HttpServer.cc.o
lib/libtmuduo_http.a: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/build.make
lib/libtmuduo_http.a: tmuduo/net/http/CMakeFiles/tmuduo_http.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/orange/tMuduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library ../../../lib/libtmuduo_http.a"
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && $(CMAKE_COMMAND) -P CMakeFiles/tmuduo_http.dir/cmake_clean_target.cmake
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tmuduo_http.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tmuduo/net/http/CMakeFiles/tmuduo_http.dir/build: lib/libtmuduo_http.a
.PHONY : tmuduo/net/http/CMakeFiles/tmuduo_http.dir/build

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/clean:
	cd /home/orange/tMuduo/build/release/tmuduo/net/http && $(CMAKE_COMMAND) -P CMakeFiles/tmuduo_http.dir/cmake_clean.cmake
.PHONY : tmuduo/net/http/CMakeFiles/tmuduo_http.dir/clean

tmuduo/net/http/CMakeFiles/tmuduo_http.dir/depend:
	cd /home/orange/tMuduo/build/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/orange/tMuduo /home/orange/tMuduo/tmuduo/net/http /home/orange/tMuduo/build/release /home/orange/tMuduo/build/release/tmuduo/net/http /home/orange/tMuduo/build/release/tmuduo/net/http/CMakeFiles/tmuduo_http.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tmuduo/net/http/CMakeFiles/tmuduo_http.dir/depend
