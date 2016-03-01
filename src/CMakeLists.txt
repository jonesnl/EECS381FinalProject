cmake_minimum_required(VERSION 2.8)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -std=c++14 -Wall -Wextra")

macro(add_gtest _name)
    add_executable(${ARGV})
    if (TARGET ${_name})
        target_link_libraries(${_name} gtest gtest_main)
    endif()
endmacro()

set(HEADER_FILES
        Collection.h
        p3_globals.h
        Record.h
        Utility.h)

set(SHARED_SOURCE_FILES
        Utility.cpp
        p3_globals.cpp
        Collection.cpp
        Record.cpp)

add_executable(p3_main ${SHARED_SOURCE_FILES} ${HEADER_FILES} p3_main.cpp)

add_gtest(CollectionTest ${SHARED_SOURCE_FILES} tests/collection_test.cpp)

add_gtest(RecordTest ${SHARED_SOURCE_FILES} tests/record_test.cpp)

add_subdirectory(googletest)

