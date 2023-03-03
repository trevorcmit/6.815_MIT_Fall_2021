# C/C++ compilation involves two stages:
# - compiling: convert each source (*.cpp) file independently into binary
#   object (.o) files
# - linking: assemble all the object files into a single executable
#
# Since any C++ project contains multiple source files, the above process
# involves firing multiple commands. Makefiles help us write all the
# compilation stages in a single file and run them as one big script.
#
# See http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/ and many
# other tutorials on makefiles.

# === Variables ================================================================

# We define some variables, so we don't repeat ourselves in the rest of the
# script. It also makes it easier to change the configuration later on if we
# decide to do so.
NUMBER := 0
CXXFLAGS := -Wall -std=c++11

# Define the files that will be compiled into the executable file
OBJS=\
$(BUILD_DIR)/a$(NUMBER).o \
$(BUILD_DIR)/Image.o \
$(BUILD_DIR)/lodepng.o

# Include build rules that are shared for all psets in this class
include Makefile.include

prepare:
	mkdir -p asst
	cp *.cpp asst
	cp *.h asst
	cp Makefile asst
	cp Makefile.include asst
	cp -R Input asst
	zip -r a$(NUMBER)_submission.zip asst
	rm -rf asst
.PHONY: prepare