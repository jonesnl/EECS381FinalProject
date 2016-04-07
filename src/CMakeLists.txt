cmake_minimum_required(VERSION 2.8)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -std=c++14 -Wall -Wextra")

set(HEADER_FILES
        Geometry.h
        Island.h
        Model.h
        Navigation.h
        Sim_object.h
        Ship.h
        Utility.h
        Track_base.h
        Ship_factory.h
        Controller.h
        View.h
        Tanker.h
        Cruise_ship.h
        Warship.h
        Cruiser.h
        Torpedo_boat.h
        Ocean_map.h
        Bridge_view.h
        Map_view.h
        Sailing_view.h
        Skimmer.h
        Ship_component.h
        Ship_composite.h)

set(SHARED_SOURCE_FILES
        Geometry.cpp
        Model.cpp
        Navigation.cpp
        Sim_object.cpp
        Island.cpp
        Ship.cpp
        Track_base.cpp
        Ship_factory.cpp
        Controller.cpp
        View.cpp
        Tanker.cpp
        Utility.cpp
        Cruise_ship.cpp
        Warship.cpp
        Cruiser.cpp
        Torpedo_boat.cpp
        Ocean_map.cpp
        Bridge_view.cpp
        Map_view.cpp
        Sailing_view.cpp
        Skimmer.cpp
        Ship_component.cpp
        Ship_composite.cpp)

macro(add_gtest _name)
    add_executable(${ARGV})
    if (TARGET ${_name})
        target_link_libraries(${_name} gtest gtest_main)
    endif()
endmacro()

add_executable(p6_main ${SHARED_SOURCE_FILES} ${HEADER_FILES} p6_main.cpp)

add_gtest(Island_test
        Island.cpp
        Sim_object.cpp
        Geometry.cpp
        test/Island_test.cpp)

add_gtest(Ship_test
        Ship.cpp
        Sim_object.cpp
        Geometry.cpp
        Track_base.cpp
        Island.cpp
        Navigation.cpp
        test/Ship_test.cpp
        test/test_utility.cpp)

add_gtest(Cruiser_test
        ${SHARED_SOURCE_FILES}
        test/Cruiser_test.cpp
        test/test_utility.cpp
        )

add_subdirectory(googletest)

