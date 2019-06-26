cmake_minimum_required(VERSION 3.0)
project(tinyobjloader)

add_library(tinyobjloader 
    include/tinyobjloader/tiny_obj_loader.h
    src/tiny_obj_loader.cc)
target_include_directories(tinyobjloader PUBLIC include/)
target_include_directories(tinyobjloader PRIVATE include/tinyobjloader)
