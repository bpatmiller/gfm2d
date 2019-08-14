default: build

.PHONY: build
build:
	-[[ -d build ]] || mkdir build
	cd build; cmake ..;  make -j8

.PHONY: format
format:
	clang-format -i src/*.cpp src/*.hpp

.PHONY: clean
clean:
	rm -rf build/
	cp -f assets/config.json.orig config.json

.PHONY: run
run:
	build/bin/gfm 

.PHONY: all
all: clean format build run