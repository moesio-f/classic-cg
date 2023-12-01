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
if (WIN32)
    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
endif (WIN32)

# Add subdirectories
add_subdirectory(core)
add_subdirectory(rendering)

# Add executables
add_executable(sdl2_debug WIN32 debug.c)
add_executable(render WIN32 render.c)

# Obtain libraries
if (WIN32)
    set(SHARED_LIBRARIES mingw32 SDL2::SDL2main)
endif (WIN32)
set(SHARED_LIBRARIES ${SHARED_LIBRARIES} SDL2::SDL2)
find_library(math m)
if(math)
    set(SHARED_LIBRARIES ${SHARED_LIBRARIES} ${math})
endif()
set(SHARED_LIBRARIES ${SHARED_LIBRARIES} core rendering)

# Link executables
target_link_libraries(sdl2_debug PRIVATE ${SHARED_LIBRARIES})
target_link_libraries(render PRIVATE ${SHARED_LIBRARIES})
