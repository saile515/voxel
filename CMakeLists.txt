cmake_minimum_required(VERSION 3.28)

project(voxel LANGUAGES C)

set(CMAKE_C_COMPILER "clang")

set(CMAKE_BUILD_TYPE "Debug")

set(PROJECT_HEADERS
    src/camera.h
    src/chunk.h
    src/load_shader.h
    src/mat4.h
    src/math_util.h
    src/read_file.h
    src/transform.h
    src/vec2.h
    src/vec3.h
    src/vector.h
    src/world.h)

set(PROJECT_SOURCES
    src/camera.c
    src/chunk.c
    src/load_shader.c
    src/mat4.c
    src/math_util.c
    src/read_file.c
    src/vec2.c
    src/vec3.c
    src/vector.c
    src/world.c)

add_executable(voxel src/main.c ${PROJECT_HEADERS} ${PROJECT_SOURCES})
target_link_libraries(voxel -lm glfw GLEW GL)
