# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_SOURCE_DIR = /home/admin/LibCLog

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/admin/LibCLog/build

# Include any dependencies generated for this target.
include CMakeFiles/libc_interceptor.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/libc_interceptor.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/libc_interceptor.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/libc_interceptor.dir/flags.make

CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.o: CMakeFiles/libc_interceptor.dir/flags.make
CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.o: /home/admin/LibCLog/libc_interceptor.cpp
CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.o: CMakeFiles/libc_interceptor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/admin/LibCLog/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.o -MF CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.o.d -o CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.o -c /home/admin/LibCLog/libc_interceptor.cpp

CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/admin/LibCLog/libc_interceptor.cpp > CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.i

CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/admin/LibCLog/libc_interceptor.cpp -o CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.s

# Object files for target libc_interceptor
libc_interceptor_OBJECTS = \
"CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.o"

# External object files for target libc_interceptor
libc_interceptor_EXTERNAL_OBJECTS =

liblibc_interceptor.so: CMakeFiles/libc_interceptor.dir/libc_interceptor.cpp.o
liblibc_interceptor.so: CMakeFiles/libc_interceptor.dir/build.make
liblibc_interceptor.so: CMakeFiles/libc_interceptor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/admin/LibCLog/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library liblibc_interceptor.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/libc_interceptor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/libc_interceptor.dir/build: liblibc_interceptor.so
.PHONY : CMakeFiles/libc_interceptor.dir/build

CMakeFiles/libc_interceptor.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/libc_interceptor.dir/cmake_clean.cmake
.PHONY : CMakeFiles/libc_interceptor.dir/clean

CMakeFiles/libc_interceptor.dir/depend:
	cd /home/admin/LibCLog/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/admin/LibCLog /home/admin/LibCLog /home/admin/LibCLog/build /home/admin/LibCLog/build /home/admin/LibCLog/build/CMakeFiles/libc_interceptor.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/libc_interceptor.dir/depend

