# Makefile to automate CMake building
#	process

# Default action is to prepare
# 	and compile all targets
all: prepare compile

# Test run the sample debug target
test: all debug

# Generate the project files with Ninja
prepare:
	@echo "[Makefile] Generating project files..."
	@cmake -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=YES -B build -S src 

# Build all targets with generated files
compile:
	@echo "[Makefile] Building all targets..."
	@cmake --build build

# Run the sample debug target
debug:
	@echo "[Makefile] Running debug target..."
	@./build/sdl2_debug

# Compile for windows
compile-windows:
	@echo "[Makefile] Compile for Windows..."
	@cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=`pwd`/toolchain/win-mingw.cmake -B build-win -S src
	@cmake --build build-win

# Prepare release
release:
	@echo "[Makefile] Compile all and zip release..."
	@make prepare compile
	@make compile-windows
	@zip -j cg-any-linux.zip build/render data/camera/* data/objects/* data/light/*
	@zip -j cg-win32-x64.zip build-win/render.exe build-win/SDL2.dll data/camera/* data/objects/* data/light/*

