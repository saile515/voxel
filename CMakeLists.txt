cmake_minimum_required(VERSION 3.28)

project(voxel LANGUAGES C)

set(CMAKE_C_COMPILER "clang")

set(PROJECT_HEADERS src/read_file.h src/load_shader.h src/mat4.h)
set(PROJECT_SOURCES src/read_file.c src/load_shader.c src/mat4.c)

add_executable(voxel src/main.c ${PROJECT_HEADERS} ${PROJECT_SOURCES})
target_link_libraries(voxel -lm glfw GLEW GL)
