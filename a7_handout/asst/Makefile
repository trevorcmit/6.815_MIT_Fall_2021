NUMBER := 7

CXXFLAGS ?= -Wall -std=c++11 -I. -w -O3

OBJS=\
$(BUILD_DIR)/basicImageManipulation.o \
$(BUILD_DIR)/filtering.o \
$(BUILD_DIR)/homography.o \
$(BUILD_DIR)/panorama.o \
$(BUILD_DIR)/blending.o \
$(BUILD_DIR)/Image.o \
$(BUILD_DIR)/lodepng.o

include Makefile.include

prepare:
	mkdir -p asst
	cp *.cpp asst
	cp *.h asst
	cp Makefile asst
	cp -R Input asst
	cp -R Eigen asst
	cp -R my_pano asst
	zip -r a$(NUMBER)_submission.zip asst
	rm -rf asst
.PHONY: prepare

