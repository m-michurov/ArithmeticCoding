# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.12

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "E:\CLion 2018.2.7\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "E:\CLion 2018.2.7\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = E:\CLionProjects\ArithmeticCoding

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = E:\CLionProjects\ArithmeticCoding\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ArithmeticCoding.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ArithmeticCoding.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ArithmeticCoding.dir/flags.make

CMakeFiles/ArithmeticCoding.dir/main.c.obj: CMakeFiles/ArithmeticCoding.dir/flags.make
CMakeFiles/ArithmeticCoding.dir/main.c.obj: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\CLionProjects\ArithmeticCoding\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ArithmeticCoding.dir/main.c.obj"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\ArithmeticCoding.dir\main.c.obj   -c E:\CLionProjects\ArithmeticCoding\main.c

CMakeFiles/ArithmeticCoding.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ArithmeticCoding.dir/main.c.i"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E E:\CLionProjects\ArithmeticCoding\main.c > CMakeFiles\ArithmeticCoding.dir\main.c.i

CMakeFiles/ArithmeticCoding.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ArithmeticCoding.dir/main.c.s"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S E:\CLionProjects\ArithmeticCoding\main.c -o CMakeFiles\ArithmeticCoding.dir\main.c.s

CMakeFiles/ArithmeticCoding.dir/binio.c.obj: CMakeFiles/ArithmeticCoding.dir/flags.make
CMakeFiles/ArithmeticCoding.dir/binio.c.obj: ../binio.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\CLionProjects\ArithmeticCoding\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/ArithmeticCoding.dir/binio.c.obj"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\ArithmeticCoding.dir\binio.c.obj   -c E:\CLionProjects\ArithmeticCoding\binio.c

CMakeFiles/ArithmeticCoding.dir/binio.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ArithmeticCoding.dir/binio.c.i"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E E:\CLionProjects\ArithmeticCoding\binio.c > CMakeFiles\ArithmeticCoding.dir\binio.c.i

CMakeFiles/ArithmeticCoding.dir/binio.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ArithmeticCoding.dir/binio.c.s"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S E:\CLionProjects\ArithmeticCoding\binio.c -o CMakeFiles\ArithmeticCoding.dir\binio.c.s

# Object files for target ArithmeticCoding
ArithmeticCoding_OBJECTS = \
"CMakeFiles/ArithmeticCoding.dir/main.c.obj" \
"CMakeFiles/ArithmeticCoding.dir/binio.c.obj"

# External object files for target ArithmeticCoding
ArithmeticCoding_EXTERNAL_OBJECTS =

ArithmeticCoding.exe: CMakeFiles/ArithmeticCoding.dir/main.c.obj
ArithmeticCoding.exe: CMakeFiles/ArithmeticCoding.dir/binio.c.obj
ArithmeticCoding.exe: CMakeFiles/ArithmeticCoding.dir/build.make
ArithmeticCoding.exe: CMakeFiles/ArithmeticCoding.dir/linklibs.rsp
ArithmeticCoding.exe: CMakeFiles/ArithmeticCoding.dir/objects1.rsp
ArithmeticCoding.exe: CMakeFiles/ArithmeticCoding.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=E:\CLionProjects\ArithmeticCoding\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable ArithmeticCoding.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\ArithmeticCoding.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ArithmeticCoding.dir/build: ArithmeticCoding.exe

.PHONY : CMakeFiles/ArithmeticCoding.dir/build

CMakeFiles/ArithmeticCoding.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\ArithmeticCoding.dir\cmake_clean.cmake
.PHONY : CMakeFiles/ArithmeticCoding.dir/clean

CMakeFiles/ArithmeticCoding.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" E:\CLionProjects\ArithmeticCoding E:\CLionProjects\ArithmeticCoding E:\CLionProjects\ArithmeticCoding\cmake-build-debug E:\CLionProjects\ArithmeticCoding\cmake-build-debug E:\CLionProjects\ArithmeticCoding\cmake-build-debug\CMakeFiles\ArithmeticCoding.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ArithmeticCoding.dir/depend

