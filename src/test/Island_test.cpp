#include "../Island.h"

#include "gtest/gtest.h"

#include <iostream>

using namespace std;

TEST(Island, basic_test) {
    Point p {1, 1};
    Island island {"Test", p, 100, 1};
    ASSERT_EQ(p, island.get_location());
    island.describe();

    ASSERT_DOUBLE_EQ(10., island.provide_fuel(10));
    island.describe();
    ASSERT_DOUBLE_EQ(90., island.provide_fuel(100));
    island.describe();
    ASSERT_DOUBLE_EQ(0., island.provide_fuel(100));

    island.update();
    island.describe();
    ASSERT_DOUBLE_EQ(1., island.provide_fuel(10));
}