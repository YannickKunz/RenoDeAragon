#!/bin/sh

emcc -o out/index.html \
	src/*.cpp \
	-Os -Wall build/external/raylib-master/src/libraylib.a \
	-s ASYNCIFY \
	-s USE_GLFW=3 \
	--shell-file build/external/raylib-master/src/minshell.html \
	-Ibuild/external/raylib-master/src/ -Isrc/ -Iinclude/ \
	-Lbuild/external/raylib-master/src/ \
	-DPLATFORM_WEB \
	-sALLOW_MEMORY_GROWTH \
	--preload-file resources\
