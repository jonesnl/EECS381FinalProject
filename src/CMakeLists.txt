cmake_minimum_required(VERSION 2.8)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -std=c++14 -Wall -Wextra")

macro(add_gtest _name)
    add_executable(${ARGV})
    if (TARGET ${_name})
        target_link_libraries(${_name} gtest gtest_main)
    endif()
endmacro()

set(HEADER_FILES Cruiser.h Geometry.h Navigation.h)

set(SHARED_SOURCE_FILES Geometry.cpp Navigation.cpp)

add_executable(p4_main ${SHARED_SOURCE_FILES} ${HEADER_FILES} p4_main.cpp)

add_subdirectory(googletest)

