# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /home/yohai/opt/clion-2017.1.1/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/yohai/opt/clion-2017.1.1/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yohai/CLionProjects/OS/Ex2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yohai/CLionProjects/OS/Ex2/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/jmp.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/jmp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/jmp.dir/flags.make

CMakeFiles/jmp.dir/demo_jmp.c.o: CMakeFiles/jmp.dir/flags.make
CMakeFiles/jmp.dir/demo_jmp.c.o: ../demo_jmp.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yohai/CLionProjects/OS/Ex2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/jmp.dir/demo_jmp.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jmp.dir/demo_jmp.c.o   -c /home/yohai/CLionProjects/OS/Ex2/demo_jmp.c

CMakeFiles/jmp.dir/demo_jmp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jmp.dir/demo_jmp.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/yohai/CLionProjects/OS/Ex2/demo_jmp.c > CMakeFiles/jmp.dir/demo_jmp.c.i

CMakeFiles/jmp.dir/demo_jmp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jmp.dir/demo_jmp.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/yohai/CLionProjects/OS/Ex2/demo_jmp.c -o CMakeFiles/jmp.dir/demo_jmp.c.s

CMakeFiles/jmp.dir/demo_jmp.c.o.requires:

.PHONY : CMakeFiles/jmp.dir/demo_jmp.c.o.requires

CMakeFiles/jmp.dir/demo_jmp.c.o.provides: CMakeFiles/jmp.dir/demo_jmp.c.o.requires
	$(MAKE) -f CMakeFiles/jmp.dir/build.make CMakeFiles/jmp.dir/demo_jmp.c.o.provides.build
.PHONY : CMakeFiles/jmp.dir/demo_jmp.c.o.provides

CMakeFiles/jmp.dir/demo_jmp.c.o.provides.build: CMakeFiles/jmp.dir/demo_jmp.c.o


# Object files for target jmp
jmp_OBJECTS = \
"CMakeFiles/jmp.dir/demo_jmp.c.o"

# External object files for target jmp
jmp_EXTERNAL_OBJECTS =

jmp: CMakeFiles/jmp.dir/demo_jmp.c.o
jmp: CMakeFiles/jmp.dir/build.make
jmp: CMakeFiles/jmp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yohai/CLionProjects/OS/Ex2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable jmp"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/jmp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/jmp.dir/build: jmp

.PHONY : CMakeFiles/jmp.dir/build

CMakeFiles/jmp.dir/requires: CMakeFiles/jmp.dir/demo_jmp.c.o.requires

.PHONY : CMakeFiles/jmp.dir/requires

CMakeFiles/jmp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/jmp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/jmp.dir/clean

CMakeFiles/jmp.dir/depend:
	cd /home/yohai/CLionProjects/OS/Ex2/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yohai/CLionProjects/OS/Ex2 /home/yohai/CLionProjects/OS/Ex2 /home/yohai/CLionProjects/OS/Ex2/cmake-build-debug /home/yohai/CLionProjects/OS/Ex2/cmake-build-debug /home/yohai/CLionProjects/OS/Ex2/cmake-build-debug/CMakeFiles/jmp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/jmp.dir/depend

