# CMakeList.txt : CMake project for GfxHair, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# include your directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    lib/glfw-3.3/include
)

add_subdirectory(lib/glad-0.1.30)

add_subdirectory(lib/glfw-3.3)

add_subdirectory(lib/tinyobjloader-1.0.7)

# Add source to this project's executable.
add_executable (GfxDemo
    "GfxDemo.cc"
    "utility.h"
    "utility.cc"
    "calc.h"
    "GfxModel.h"
    "GfxModel.cc"
    "GfxShader.h"
    "GfxShader.cc")

target_link_libraries(GfxDemo glad glfw ${GLFW_LIBRARIES} tinyobjloader)

# TODO: Add tests and install targets if needed.

install(TARGETS GfxDemo
    DESTINATION bin)
