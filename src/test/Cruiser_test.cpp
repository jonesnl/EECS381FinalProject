#include "../Cruiser.h"
#include "../Model.h"
#include "../Island.h"
#include "test_utility.h"

#include "gtest/gtest.h"

TEST (CruiserTest, basic) {
    set_fp_cout();
    Cruiser ship("Test", {0., 0.});
    Island island("HI", {0, 0});
    ASSERT_TRUE(ship.is_afloat());
    ASSERT_FALSE(ship.is_on_the_bottom());
    ASSERT_FALSE(ship.is_moving());
    ASSERT_FALSE(ship.is_docked());
    ASSERT_TRUE(ship.can_dock(&island));
    ship.update();
    ship.attak;
    ship.update();
}

