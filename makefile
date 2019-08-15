default: build

.PHONY: build
build:
	rm -rf build/CMakeFiles/gfm.dir
	-[[ -f config.json ]] || cp assets/config.json.orig config.json
	-[[ -d build ]] || mkdir build
	cd build; cmake ..;  make -j8

.PHONY: format
format:
	clang-format -i src/*.cpp src/*.hpp
	#test/*.cpp test/*.hpp

.PHONY: clean
clean:
	rm -rf build/
	cp -f assets/config.json.orig config.json
	rm -f ./*.gcov
	rm -f capture.info

.PHONY: run
run:
	build/bin/gfm 

.PHONY: test
test: clean build
	build/bin/unit-tests

.PHONY: docs
docs:
	doxygen .doxyfile

.PHONY: all
all: clean format build run