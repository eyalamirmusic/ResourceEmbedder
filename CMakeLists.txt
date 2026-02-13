cmake_minimum_required(VERSION 4.1)
project(ResourceEmbedder)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Lib)
add_executable(ResourceEmbedder main.cpp Binary.cpp)
target_link_libraries(ResourceEmbedder PRIVATE ResourceEmbedLib)