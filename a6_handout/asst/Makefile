NUMBER := 6

OBJS=\
$(BUILD_DIR)/basicImageManipulation.o \
$(BUILD_DIR)/homography.o \
$(BUILD_DIR)/Image.o \
$(BUILD_DIR)/lodepng.o

include Makefile.include

# special perpare rule for this pset. Include Eigen and class_morph directories
prepare:
	mkdir -p asst
	cp *.cpp asst
	cp *.h asst
	[ ! -f *.png ] || cp *.png asst
	cp Makefile asst
	cp Makefile.include asst
	cp -R Input asst
	cp -R Eigen asst
	cp -R class_morph asst
	cp segments.txt asst
	zip -r a$(NUMBER)_submission.zip asst
	rm -rf asst
.PHONY: prepare
