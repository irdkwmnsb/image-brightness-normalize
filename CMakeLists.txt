cmake_minimum_required(VERSION 3.17)
project(hw5 C)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-static)
endif()

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
    message(FATAL_ERROR "OpenMP is not found!")
endif ()

add_executable(hw5 main.c)