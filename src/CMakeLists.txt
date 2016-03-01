cmake_minimum_required(VERSION 2.8)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -std=c++14 -Wall -Wextra")

set(HEADER_FILES Cruiser.h
        Geometry.h
        Island.h
        Model.h
        Navigation.h
        Sim_object.h
        Ship.h
        Track_base.h)

set(SHARED_SOURCE_FILES
        Geometry.cpp
        Model.cpp
        Navigation.cpp
        Sim_object.cpp
        Island.cpp
        Ship.cpp
        Track_base.cpp)

macro(add_gtest _name)
    add_executable(${ARGV})
    if (TARGET ${_name})
        target_link_libraries(${_name} gtest gtest_main)
    endif()
endmacro()

add_executable(p4_main ${SHARED_SOURCE_FILES} ${HEADER_FILES} p4_main.cpp)

add_gtest(Island_test
        Island.cpp
        Sim_object.cpp
        Geometry.cpp
        test/Island_test.cpp)

add_subdirectory(googletest)

