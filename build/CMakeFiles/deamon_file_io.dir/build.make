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
include CMakeFiles/deamon_file_io.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/deamon_file_io.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/deamon_file_io.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/deamon_file_io.dir/flags.make

CMakeFiles/deamon_file_io.dir/daemon_file_io.c.o: CMakeFiles/deamon_file_io.dir/flags.make
CMakeFiles/deamon_file_io.dir/daemon_file_io.c.o: /home/admin/LibCLog/daemon_file_io.c
CMakeFiles/deamon_file_io.dir/daemon_file_io.c.o: CMakeFiles/deamon_file_io.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/admin/LibCLog/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/deamon_file_io.dir/daemon_file_io.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/deamon_file_io.dir/daemon_file_io.c.o -MF CMakeFiles/deamon_file_io.dir/daemon_file_io.c.o.d -o CMakeFiles/deamon_file_io.dir/daemon_file_io.c.o -c /home/admin/LibCLog/daemon_file_io.c

CMakeFiles/deamon_file_io.dir/daemon_file_io.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/deamon_file_io.dir/daemon_file_io.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/admin/LibCLog/daemon_file_io.c > CMakeFiles/deamon_file_io.dir/daemon_file_io.c.i

CMakeFiles/deamon_file_io.dir/daemon_file_io.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/deamon_file_io.dir/daemon_file_io.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/admin/LibCLog/daemon_file_io.c -o CMakeFiles/deamon_file_io.dir/daemon_file_io.c.s

# Object files for target deamon_file_io
deamon_file_io_OBJECTS = \
"CMakeFiles/deamon_file_io.dir/daemon_file_io.c.o"

# External object files for target deamon_file_io
deamon_file_io_EXTERNAL_OBJECTS =

deamon_file_io: CMakeFiles/deamon_file_io.dir/daemon_file_io.c.o
deamon_file_io: CMakeFiles/deamon_file_io.dir/build.make
deamon_file_io: liblibc_wrapper.so
deamon_file_io: CMakeFiles/deamon_file_io.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/admin/LibCLog/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable deamon_file_io"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/deamon_file_io.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/deamon_file_io.dir/build: deamon_file_io
.PHONY : CMakeFiles/deamon_file_io.dir/build

CMakeFiles/deamon_file_io.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/deamon_file_io.dir/cmake_clean.cmake
.PHONY : CMakeFiles/deamon_file_io.dir/clean

CMakeFiles/deamon_file_io.dir/depend:
	cd /home/admin/LibCLog/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/admin/LibCLog /home/admin/LibCLog /home/admin/LibCLog/build /home/admin/LibCLog/build /home/admin/LibCLog/build/CMakeFiles/deamon_file_io.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/deamon_file_io.dir/depend

