# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/tb6

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/tb6

# Include any dependencies generated for this target.
include CMakeFiles/camcv.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/camcv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/camcv.dir/flags.make

CMakeFiles/camcv.dir/Client.cpp.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/Client.cpp.o: Client.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/camcv.dir/Client.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/camcv.dir/Client.cpp.o -c /home/pi/tb6/Client.cpp

CMakeFiles/camcv.dir/Client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/camcv.dir/Client.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/pi/tb6/Client.cpp > CMakeFiles/camcv.dir/Client.cpp.i

CMakeFiles/camcv.dir/Client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/camcv.dir/Client.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/pi/tb6/Client.cpp -o CMakeFiles/camcv.dir/Client.cpp.s

CMakeFiles/camcv.dir/Client.cpp.o.requires:
.PHONY : CMakeFiles/camcv.dir/Client.cpp.o.requires

CMakeFiles/camcv.dir/Client.cpp.o.provides: CMakeFiles/camcv.dir/Client.cpp.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/Client.cpp.o.provides.build
.PHONY : CMakeFiles/camcv.dir/Client.cpp.o.provides

CMakeFiles/camcv.dir/Client.cpp.o.provides.build: CMakeFiles/camcv.dir/Client.cpp.o

CMakeFiles/camcv.dir/OCR.cpp.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/OCR.cpp.o: OCR.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/camcv.dir/OCR.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/camcv.dir/OCR.cpp.o -c /home/pi/tb6/OCR.cpp

CMakeFiles/camcv.dir/OCR.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/camcv.dir/OCR.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/pi/tb6/OCR.cpp > CMakeFiles/camcv.dir/OCR.cpp.i

CMakeFiles/camcv.dir/OCR.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/camcv.dir/OCR.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/pi/tb6/OCR.cpp -o CMakeFiles/camcv.dir/OCR.cpp.s

CMakeFiles/camcv.dir/OCR.cpp.o.requires:
.PHONY : CMakeFiles/camcv.dir/OCR.cpp.o.requires

CMakeFiles/camcv.dir/OCR.cpp.o.provides: CMakeFiles/camcv.dir/OCR.cpp.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/OCR.cpp.o.provides.build
.PHONY : CMakeFiles/camcv.dir/OCR.cpp.o.provides

CMakeFiles/camcv.dir/OCR.cpp.o.provides.build: CMakeFiles/camcv.dir/OCR.cpp.o

CMakeFiles/camcv.dir/CamCV.cpp.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/CamCV.cpp.o: CamCV.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/camcv.dir/CamCV.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/camcv.dir/CamCV.cpp.o -c /home/pi/tb6/CamCV.cpp

CMakeFiles/camcv.dir/CamCV.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/camcv.dir/CamCV.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/pi/tb6/CamCV.cpp > CMakeFiles/camcv.dir/CamCV.cpp.i

CMakeFiles/camcv.dir/CamCV.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/camcv.dir/CamCV.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/pi/tb6/CamCV.cpp -o CMakeFiles/camcv.dir/CamCV.cpp.s

CMakeFiles/camcv.dir/CamCV.cpp.o.requires:
.PHONY : CMakeFiles/camcv.dir/CamCV.cpp.o.requires

CMakeFiles/camcv.dir/CamCV.cpp.o.provides: CMakeFiles/camcv.dir/CamCV.cpp.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/CamCV.cpp.o.provides.build
.PHONY : CMakeFiles/camcv.dir/CamCV.cpp.o.provides

CMakeFiles/camcv.dir/CamCV.cpp.o.provides.build: CMakeFiles/camcv.dir/CamCV.cpp.o

CMakeFiles/camcv.dir/RaspiCamControl.c.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/RaspiCamControl.c.o: RaspiCamControl.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/camcv.dir/RaspiCamControl.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/camcv.dir/RaspiCamControl.c.o   -c /home/pi/tb6/RaspiCamControl.c

CMakeFiles/camcv.dir/RaspiCamControl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/camcv.dir/RaspiCamControl.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/pi/tb6/RaspiCamControl.c > CMakeFiles/camcv.dir/RaspiCamControl.c.i

CMakeFiles/camcv.dir/RaspiCamControl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/camcv.dir/RaspiCamControl.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/pi/tb6/RaspiCamControl.c -o CMakeFiles/camcv.dir/RaspiCamControl.c.s

CMakeFiles/camcv.dir/RaspiCamControl.c.o.requires:
.PHONY : CMakeFiles/camcv.dir/RaspiCamControl.c.o.requires

CMakeFiles/camcv.dir/RaspiCamControl.c.o.provides: CMakeFiles/camcv.dir/RaspiCamControl.c.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/RaspiCamControl.c.o.provides.build
.PHONY : CMakeFiles/camcv.dir/RaspiCamControl.c.o.provides

CMakeFiles/camcv.dir/RaspiCamControl.c.o.provides.build: CMakeFiles/camcv.dir/RaspiCamControl.c.o

CMakeFiles/camcv.dir/RaspiCLI.c.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/RaspiCLI.c.o: RaspiCLI.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/camcv.dir/RaspiCLI.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/camcv.dir/RaspiCLI.c.o   -c /home/pi/tb6/RaspiCLI.c

CMakeFiles/camcv.dir/RaspiCLI.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/camcv.dir/RaspiCLI.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/pi/tb6/RaspiCLI.c > CMakeFiles/camcv.dir/RaspiCLI.c.i

CMakeFiles/camcv.dir/RaspiCLI.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/camcv.dir/RaspiCLI.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/pi/tb6/RaspiCLI.c -o CMakeFiles/camcv.dir/RaspiCLI.c.s

CMakeFiles/camcv.dir/RaspiCLI.c.o.requires:
.PHONY : CMakeFiles/camcv.dir/RaspiCLI.c.o.requires

CMakeFiles/camcv.dir/RaspiCLI.c.o.provides: CMakeFiles/camcv.dir/RaspiCLI.c.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/RaspiCLI.c.o.provides.build
.PHONY : CMakeFiles/camcv.dir/RaspiCLI.c.o.provides

CMakeFiles/camcv.dir/RaspiCLI.c.o.provides.build: CMakeFiles/camcv.dir/RaspiCLI.c.o

CMakeFiles/camcv.dir/RaspiPreview.c.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/RaspiPreview.c.o: RaspiPreview.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/camcv.dir/RaspiPreview.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/camcv.dir/RaspiPreview.c.o   -c /home/pi/tb6/RaspiPreview.c

CMakeFiles/camcv.dir/RaspiPreview.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/camcv.dir/RaspiPreview.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/pi/tb6/RaspiPreview.c > CMakeFiles/camcv.dir/RaspiPreview.c.i

CMakeFiles/camcv.dir/RaspiPreview.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/camcv.dir/RaspiPreview.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/pi/tb6/RaspiPreview.c -o CMakeFiles/camcv.dir/RaspiPreview.c.s

CMakeFiles/camcv.dir/RaspiPreview.c.o.requires:
.PHONY : CMakeFiles/camcv.dir/RaspiPreview.c.o.requires

CMakeFiles/camcv.dir/RaspiPreview.c.o.provides: CMakeFiles/camcv.dir/RaspiPreview.c.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/RaspiPreview.c.o.provides.build
.PHONY : CMakeFiles/camcv.dir/RaspiPreview.c.o.provides

CMakeFiles/camcv.dir/RaspiPreview.c.o.provides.build: CMakeFiles/camcv.dir/RaspiPreview.c.o

CMakeFiles/camcv.dir/RaspiTex.c.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/RaspiTex.c.o: RaspiTex.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/camcv.dir/RaspiTex.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/camcv.dir/RaspiTex.c.o   -c /home/pi/tb6/RaspiTex.c

CMakeFiles/camcv.dir/RaspiTex.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/camcv.dir/RaspiTex.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/pi/tb6/RaspiTex.c > CMakeFiles/camcv.dir/RaspiTex.c.i

CMakeFiles/camcv.dir/RaspiTex.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/camcv.dir/RaspiTex.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/pi/tb6/RaspiTex.c -o CMakeFiles/camcv.dir/RaspiTex.c.s

CMakeFiles/camcv.dir/RaspiTex.c.o.requires:
.PHONY : CMakeFiles/camcv.dir/RaspiTex.c.o.requires

CMakeFiles/camcv.dir/RaspiTex.c.o.provides: CMakeFiles/camcv.dir/RaspiTex.c.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/RaspiTex.c.o.provides.build
.PHONY : CMakeFiles/camcv.dir/RaspiTex.c.o.provides

CMakeFiles/camcv.dir/RaspiTex.c.o.provides.build: CMakeFiles/camcv.dir/RaspiTex.c.o

CMakeFiles/camcv.dir/RaspiTexUtil.c.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/RaspiTexUtil.c.o: RaspiTexUtil.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/camcv.dir/RaspiTexUtil.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/camcv.dir/RaspiTexUtil.c.o   -c /home/pi/tb6/RaspiTexUtil.c

CMakeFiles/camcv.dir/RaspiTexUtil.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/camcv.dir/RaspiTexUtil.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/pi/tb6/RaspiTexUtil.c > CMakeFiles/camcv.dir/RaspiTexUtil.c.i

CMakeFiles/camcv.dir/RaspiTexUtil.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/camcv.dir/RaspiTexUtil.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/pi/tb6/RaspiTexUtil.c -o CMakeFiles/camcv.dir/RaspiTexUtil.c.s

CMakeFiles/camcv.dir/RaspiTexUtil.c.o.requires:
.PHONY : CMakeFiles/camcv.dir/RaspiTexUtil.c.o.requires

CMakeFiles/camcv.dir/RaspiTexUtil.c.o.provides: CMakeFiles/camcv.dir/RaspiTexUtil.c.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/RaspiTexUtil.c.o.provides.build
.PHONY : CMakeFiles/camcv.dir/RaspiTexUtil.c.o.provides

CMakeFiles/camcv.dir/RaspiTexUtil.c.o.provides.build: CMakeFiles/camcv.dir/RaspiTexUtil.c.o

CMakeFiles/camcv.dir/teapot.c.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/teapot.c.o: teapot.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_9)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/camcv.dir/teapot.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/camcv.dir/teapot.c.o   -c /home/pi/tb6/teapot.c

CMakeFiles/camcv.dir/teapot.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/camcv.dir/teapot.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/pi/tb6/teapot.c > CMakeFiles/camcv.dir/teapot.c.i

CMakeFiles/camcv.dir/teapot.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/camcv.dir/teapot.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/pi/tb6/teapot.c -o CMakeFiles/camcv.dir/teapot.c.s

CMakeFiles/camcv.dir/teapot.c.o.requires:
.PHONY : CMakeFiles/camcv.dir/teapot.c.o.requires

CMakeFiles/camcv.dir/teapot.c.o.provides: CMakeFiles/camcv.dir/teapot.c.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/teapot.c.o.provides.build
.PHONY : CMakeFiles/camcv.dir/teapot.c.o.provides

CMakeFiles/camcv.dir/teapot.c.o.provides.build: CMakeFiles/camcv.dir/teapot.c.o

CMakeFiles/camcv.dir/models.c.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/models.c.o: models.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_10)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/camcv.dir/models.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/camcv.dir/models.c.o   -c /home/pi/tb6/models.c

CMakeFiles/camcv.dir/models.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/camcv.dir/models.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/pi/tb6/models.c > CMakeFiles/camcv.dir/models.c.i

CMakeFiles/camcv.dir/models.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/camcv.dir/models.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/pi/tb6/models.c -o CMakeFiles/camcv.dir/models.c.s

CMakeFiles/camcv.dir/models.c.o.requires:
.PHONY : CMakeFiles/camcv.dir/models.c.o.requires

CMakeFiles/camcv.dir/models.c.o.provides: CMakeFiles/camcv.dir/models.c.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/models.c.o.provides.build
.PHONY : CMakeFiles/camcv.dir/models.c.o.provides

CMakeFiles/camcv.dir/models.c.o.provides.build: CMakeFiles/camcv.dir/models.c.o

CMakeFiles/camcv.dir/square.c.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/square.c.o: square.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_11)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/camcv.dir/square.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/camcv.dir/square.c.o   -c /home/pi/tb6/square.c

CMakeFiles/camcv.dir/square.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/camcv.dir/square.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/pi/tb6/square.c > CMakeFiles/camcv.dir/square.c.i

CMakeFiles/camcv.dir/square.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/camcv.dir/square.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/pi/tb6/square.c -o CMakeFiles/camcv.dir/square.c.s

CMakeFiles/camcv.dir/square.c.o.requires:
.PHONY : CMakeFiles/camcv.dir/square.c.o.requires

CMakeFiles/camcv.dir/square.c.o.provides: CMakeFiles/camcv.dir/square.c.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/square.c.o.provides.build
.PHONY : CMakeFiles/camcv.dir/square.c.o.provides

CMakeFiles/camcv.dir/square.c.o.provides.build: CMakeFiles/camcv.dir/square.c.o

CMakeFiles/camcv.dir/mirror.c.o: CMakeFiles/camcv.dir/flags.make
CMakeFiles/camcv.dir/mirror.c.o: mirror.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/tb6/CMakeFiles $(CMAKE_PROGRESS_12)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/camcv.dir/mirror.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/camcv.dir/mirror.c.o   -c /home/pi/tb6/mirror.c

CMakeFiles/camcv.dir/mirror.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/camcv.dir/mirror.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/pi/tb6/mirror.c > CMakeFiles/camcv.dir/mirror.c.i

CMakeFiles/camcv.dir/mirror.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/camcv.dir/mirror.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/pi/tb6/mirror.c -o CMakeFiles/camcv.dir/mirror.c.s

CMakeFiles/camcv.dir/mirror.c.o.requires:
.PHONY : CMakeFiles/camcv.dir/mirror.c.o.requires

CMakeFiles/camcv.dir/mirror.c.o.provides: CMakeFiles/camcv.dir/mirror.c.o.requires
	$(MAKE) -f CMakeFiles/camcv.dir/build.make CMakeFiles/camcv.dir/mirror.c.o.provides.build
.PHONY : CMakeFiles/camcv.dir/mirror.c.o.provides

CMakeFiles/camcv.dir/mirror.c.o.provides.build: CMakeFiles/camcv.dir/mirror.c.o

# Object files for target camcv
camcv_OBJECTS = \
"CMakeFiles/camcv.dir/Client.cpp.o" \
"CMakeFiles/camcv.dir/OCR.cpp.o" \
"CMakeFiles/camcv.dir/CamCV.cpp.o" \
"CMakeFiles/camcv.dir/RaspiCamControl.c.o" \
"CMakeFiles/camcv.dir/RaspiCLI.c.o" \
"CMakeFiles/camcv.dir/RaspiPreview.c.o" \
"CMakeFiles/camcv.dir/RaspiTex.c.o" \
"CMakeFiles/camcv.dir/RaspiTexUtil.c.o" \
"CMakeFiles/camcv.dir/teapot.c.o" \
"CMakeFiles/camcv.dir/models.c.o" \
"CMakeFiles/camcv.dir/square.c.o" \
"CMakeFiles/camcv.dir/mirror.c.o"

# External object files for target camcv
camcv_EXTERNAL_OBJECTS =

camcv: CMakeFiles/camcv.dir/Client.cpp.o
camcv: CMakeFiles/camcv.dir/OCR.cpp.o
camcv: CMakeFiles/camcv.dir/CamCV.cpp.o
camcv: CMakeFiles/camcv.dir/RaspiCamControl.c.o
camcv: CMakeFiles/camcv.dir/RaspiCLI.c.o
camcv: CMakeFiles/camcv.dir/RaspiPreview.c.o
camcv: CMakeFiles/camcv.dir/RaspiTex.c.o
camcv: CMakeFiles/camcv.dir/RaspiTexUtil.c.o
camcv: CMakeFiles/camcv.dir/teapot.c.o
camcv: CMakeFiles/camcv.dir/models.c.o
camcv: CMakeFiles/camcv.dir/square.c.o
camcv: CMakeFiles/camcv.dir/mirror.c.o
camcv: CMakeFiles/camcv.dir/build.make
camcv: /opt/vc/lib/libmmal_core.so
camcv: /opt/vc/lib/libmmal_util.so
camcv: /opt/vc/lib/libmmal_vc_client.so
camcv: /opt/vc/lib/libvcos.so
camcv: /opt/vc/lib/libbcm_host.so
camcv: /opt/vc/lib/libGLESv2.so
camcv: /opt/vc/lib/libEGL.so
camcv: CMakeFiles/camcv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable camcv"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/camcv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/camcv.dir/build: camcv
.PHONY : CMakeFiles/camcv.dir/build

CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/Client.cpp.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/OCR.cpp.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/CamCV.cpp.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/RaspiCamControl.c.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/RaspiCLI.c.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/RaspiPreview.c.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/RaspiTex.c.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/RaspiTexUtil.c.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/teapot.c.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/models.c.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/square.c.o.requires
CMakeFiles/camcv.dir/requires: CMakeFiles/camcv.dir/mirror.c.o.requires
.PHONY : CMakeFiles/camcv.dir/requires

CMakeFiles/camcv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/camcv.dir/cmake_clean.cmake
.PHONY : CMakeFiles/camcv.dir/clean

CMakeFiles/camcv.dir/depend:
	cd /home/pi/tb6 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/tb6 /home/pi/tb6 /home/pi/tb6 /home/pi/tb6 /home/pi/tb6/CMakeFiles/camcv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/camcv.dir/depend
