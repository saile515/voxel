cmake_minimum_required(VERSION 3.28)

project(voxel LANGUAGES C)

set(CMAKE_C_COMPILER "clang")

set(PROJECT_HEADERS
    src/camera.h
    src/load_shader.h
    src/mat4.h
    src/read_file.h
    src/transform.h
    src/vec2.h
    src/vec3.h)

set(PROJECT_SOURCES src/camera.c src/load_shader.c src/mat4.c src/read_file.c
                    src/vec2.c src/vec3.c)

add_executable(voxel src/main.c ${PROJECT_HEADERS} ${PROJECT_SOURCES})
target_link_libraries(voxel -lm glfw GLEW GL)
