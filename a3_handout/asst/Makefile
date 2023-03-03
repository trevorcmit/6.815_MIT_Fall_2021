# Set -O3 for this pset to speed up align()

NUMBER := 3
CXXFLAGS ?= -Wall -std=c++11 -I. -w -O3
OBJS=\
$(BUILD_DIR)/basicImageManipulation.o \
$(BUILD_DIR)/align.o \
$(BUILD_DIR)/demosaic.o \
$(BUILD_DIR)/Image.o \
$(BUILD_DIR)/lodepng.o

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