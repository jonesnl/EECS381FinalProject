#include "../Ship.h"
#include "test_utility.h"

#include "gtest/gtest.h"

TEST (ShipTest, basic) {
    set_fp_cout();
    Ship ship("Test", {0., 0.}, 100, 100, 1, 100);
    ASSERT_FALSE(ship.is_moving());
    ship.update();
    ship.set_course_and_speed(0., 1.);
    ship.update();
}

TEST (ShipTest, out_of_fuel) {
    set_fp_cout();
    Ship barge("Barge", {0., 0.}, 100, 100, 75, 100);
    barge.set_course_and_speed(0., 1.);
    barge.update();
    barge.update();
    ASSERT_FALSE(barge.can_move());
}

TEST (ShipTest, sinking) {
    set_fp_cout();
    Ship ship("Test", {0., 0.}, 100, 100, 1, 100);
    Ship ship2("Attacker", {0., 1.}, 100, 100, 1, 100);
    ship.receive_hit(10, &ship2);
    ship.receive_hit(80, &ship2);
    ship.receive_hit(11, &ship2);
    ship.update();
    ship.describe();
}