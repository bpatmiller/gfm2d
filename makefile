default: build

.PHONY: build
build:
	rm -rf build/CMakeFiles/gfm.dir
	-[[ -f config.json ]] || cp assets/config.json.orig config.json
	-[[ -d build ]] || mkdir build
	cd build; cmake ..;  make -j8

.PHONY: format
format:
	find ./src ./test -iname *.hpp -o -iname *.cpp \
	| xargs clang-format -i
	cmake-format -i --command-case canonical --keyword-case upper \
	--enable-sort True --autosort True --enable-markup True \
	./CMakeLists.txt ./test/CMakeLists.txt

.PHONY: clean
clean:
	rm -f plot/data/*
	rm -f plot/output/*
	rm -rf build/
	cp -f assets/config.json.orig config.json
	rm -f ./*.gcov
	rm -f capture.info

.PHONY: run
run:
	build/bin/gfm 

.PHONY: test
test: build
	build/bin/unit-tests

.PHONY: docs
docs:
	rm -rf docs/ && doxygen .doxyfile

.PHONY: draw
draw:
	gnuplot plot/draw_phi.gnu

.PHONY: all
all: clean format build run