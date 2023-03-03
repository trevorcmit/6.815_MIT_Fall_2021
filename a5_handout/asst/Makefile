NUMBER := 5
OBJS=\
$(BUILD_DIR)/basicImageManipulation.o \
$(BUILD_DIR)/morphing.o \
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
	cp Input/myface.png asst/Input
	zip -r a$(NUMBER)_submission.zip asst
	rm -rf asst
.PHONY: prepare
