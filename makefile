default: build

.PHONY: build
build:
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

.PHONY: unit
unit:
	build/bin/unit-tests

.PHONY: all
all: clean format build run