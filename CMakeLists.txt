cmake_minimum_required(VERSION 3.6)
project(TerrainPain)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp)
add_executable(TerrainPain ${SOURCE_FILES})

include_directories(. dependencies/include)

target_link_libraries(TerrainPain /home/matt/SoftwareProjects/C++/TerrainPain/dependencies/libMathernoGL.a GL GLEW glfw freeimage)