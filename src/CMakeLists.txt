# CMake policy
cmake_minimum_required(VERSION 3.20)

# Setting project name, version and language
project(ComputerGraphics
        VERSION 1.0
        LANGUAGES C)

# Find system-wide SDL2 installation
# TODO: potentially add bundled SDL2 version
#   in an `external` directory (maybe submodules)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

# Add SDL2 debug executable
add_executable(sdl2_debug WIN32 debug.c)

# Link executable to SDL2
target_link_libraries(sdl2_debug PRIVATE SDL2::SDL2)

