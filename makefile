default: build

.PHONY: build
build:
	rm -rf build/CMakeFiles/gfm.dir
	-[[ -f config.json ]] || cp assets/config.json.orig config.json
	-[[ -d build ]] || mkdir build
	cd build; cmake ..;  make -j8

.PHONY: format
format:
	find ./lib ./src ./test -iname *.hpp -o -iname *.cpp \
	| xargs clang-format -i
	cmake-format -i --command-case canonical --keyword-case upper \
	--enable-sort True --autosort True --enable-markup True \
	./CMakeLists.txt ./src/CMakeLists.txt ./lib/CMakeLists.txt ./test/CMakeLists.txt
	autopep8 --in-place --aggressive --aggressive plot/plot.py
	python -m json.tool assets/config.json.orig assets/tmp.json
	mv -f assets/tmp.json assets/config.json.orig
	python -m json.tool config.json tmp.json
	mv -f tmp.json config.json

.PHONY: clean
clean:
	rm -f plot/data/*
	rm -f plot/images/*
	rm -rf build/
	rm -f ./*.gcov
	rm -f capture.info

.PHONY: run
run:
	build/bin/gfm 

.PHONY: test
test:
	build/bin/gfm_test

.PHONY: docs
docs:
	rm -rf docs/ && doxygen .doxyfile

.PHONY: draw
draw:
	python plot/plot.py

.PHONY: movie
movie:
	python plot/movie.py
	ffmpeg -r 30 -i plot/images/phi%05d.png -c:v libvpx-vp9 -crf 30 -b:v 0 -y -an video.webm -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2"
	mpv video.webm

.PHONY: all
all: clean format build test

#note - this depends on using clang, added this just for testing on the macbook
.PHONY: profile
profile: format clean build
	LLVM_PROFILE_FILE="code-%p.profraw" build/bin/gfm
	cp -f examples/dam_break.json config.json
	LLVM_PROFILE_FILE="code-%p.profraw" build/bin/gfm
	llvm-profdata merge -output=code.profdata code-*.profraw
	llvm-cov show -format=html -instr-profile code.profdata build/bin/gfm > report.html