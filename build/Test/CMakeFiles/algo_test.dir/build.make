# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yuche/MySTL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yuche/MySTL/build

# Include any dependencies generated for this target.
include Test/CMakeFiles/algo_test.dir/depend.make

# Include the progress variables for this target.
include Test/CMakeFiles/algo_test.dir/progress.make

# Include the compile flags for this target's objects.
include Test/CMakeFiles/algo_test.dir/flags.make

Test/CMakeFiles/algo_test.dir/algo_test.cpp.o: Test/CMakeFiles/algo_test.dir/flags.make
Test/CMakeFiles/algo_test.dir/algo_test.cpp.o: ../Test/algo_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yuche/MySTL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Test/CMakeFiles/algo_test.dir/algo_test.cpp.o"
	cd /home/yuche/MySTL/build/Test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/algo_test.dir/algo_test.cpp.o -c /home/yuche/MySTL/Test/algo_test.cpp

Test/CMakeFiles/algo_test.dir/algo_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/algo_test.dir/algo_test.cpp.i"
	cd /home/yuche/MySTL/build/Test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yuche/MySTL/Test/algo_test.cpp > CMakeFiles/algo_test.dir/algo_test.cpp.i

Test/CMakeFiles/algo_test.dir/algo_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/algo_test.dir/algo_test.cpp.s"
	cd /home/yuche/MySTL/build/Test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yuche/MySTL/Test/algo_test.cpp -o CMakeFiles/algo_test.dir/algo_test.cpp.s

Test/CMakeFiles/algo_test.dir/__/src/alloc.cpp.o: Test/CMakeFiles/algo_test.dir/flags.make
Test/CMakeFiles/algo_test.dir/__/src/alloc.cpp.o: ../src/alloc.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yuche/MySTL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object Test/CMakeFiles/algo_test.dir/__/src/alloc.cpp.o"
	cd /home/yuche/MySTL/build/Test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/algo_test.dir/__/src/alloc.cpp.o -c /home/yuche/MySTL/src/alloc.cpp

Test/CMakeFiles/algo_test.dir/__/src/alloc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/algo_test.dir/__/src/alloc.cpp.i"
	cd /home/yuche/MySTL/build/Test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yuche/MySTL/src/alloc.cpp > CMakeFiles/algo_test.dir/__/src/alloc.cpp.i

Test/CMakeFiles/algo_test.dir/__/src/alloc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/algo_test.dir/__/src/alloc.cpp.s"
	cd /home/yuche/MySTL/build/Test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yuche/MySTL/src/alloc.cpp -o CMakeFiles/algo_test.dir/__/src/alloc.cpp.s

# Object files for target algo_test
algo_test_OBJECTS = \
"CMakeFiles/algo_test.dir/algo_test.cpp.o" \
"CMakeFiles/algo_test.dir/__/src/alloc.cpp.o"

# External object files for target algo_test
algo_test_EXTERNAL_OBJECTS =

Test/algo_test: Test/CMakeFiles/algo_test.dir/algo_test.cpp.o
Test/algo_test: Test/CMakeFiles/algo_test.dir/__/src/alloc.cpp.o
Test/algo_test: Test/CMakeFiles/algo_test.dir/build.make
Test/algo_test: Test/CMakeFiles/algo_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yuche/MySTL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable algo_test"
	cd /home/yuche/MySTL/build/Test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/algo_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Test/CMakeFiles/algo_test.dir/build: Test/algo_test

.PHONY : Test/CMakeFiles/algo_test.dir/build

Test/CMakeFiles/algo_test.dir/clean:
	cd /home/yuche/MySTL/build/Test && $(CMAKE_COMMAND) -P CMakeFiles/algo_test.dir/cmake_clean.cmake
.PHONY : Test/CMakeFiles/algo_test.dir/clean

Test/CMakeFiles/algo_test.dir/depend:
	cd /home/yuche/MySTL/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yuche/MySTL /home/yuche/MySTL/Test /home/yuche/MySTL/build /home/yuche/MySTL/build/Test /home/yuche/MySTL/build/Test/CMakeFiles/algo_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Test/CMakeFiles/algo_test.dir/depend
