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
	@cmake -G "Ninja" -B build -S src

# Build all targets with generated files
compile:
	@echo "[Makefile] Building all targets..."
	@cmake --build build

# Run the sample debug target
debug:
	@echo "[Makefile] Running debug target..."
	@./build/sdl2_debug
