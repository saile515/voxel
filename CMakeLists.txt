cmake_minimum_required(VERSION 3.28)

project(voxel LANGUAGES C CXX)

# if(NOT DEFINED CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Debug")
# endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  option(TRACY_ENABLE ON)
  option(TRACY_ON_DEMAND ON)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_C_FLAGS_DEBUG -g)
endif()

add_subdirectory(tracy)

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
target_link_libraries(voxel -lm glfw GLEW GL Tracy::TracyClient)
target_include_directories(voxel PRIVATE tracy/public)
