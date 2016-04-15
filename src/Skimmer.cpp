#include "Skimmer.h"
#include "Utility.h"

#include <iostream>
#include <cassert>

using namespace std;

const double skimmer_fuel_capacity_c = 100.;
const double skimmer_maximum_speed_c = 2.;
const double skimmer_fuel_consumption_c = 0.5;
const int skimmer_resistance_c = 0;

/* Tracking the Skimming State
 * We track where we are in the state of skimming the requested area by
 * keeping track of how many sides of the spiral we still need to skim.
 * From the number of sides to skim, we can calculate how far we need to
 * travel for this edge of the spiral pattern. The direction of travel is
 * tracked using a state machine.
 */

// Construct a skimmer object
Skimmer::Skimmer(const string &name_, Point position_) :
        Ship(name_, position_, skimmer_fuel_capacity_c,
                skimmer_maximum_speed_c, skimmer_fuel_consumption_c,
                skimmer_resistance_c),
        skimming_state(SkimmingState_t::not_skimming) { }

// Describe the skimming object
void Skimmer::describe() const {
    cout << "\nSkimmer ";
    Ship::describe();
    if (is_skimming())
        cout << "Is skimming a spill of size " << spill_size <<
                " starting at " << spill_sw_corner << endl;
}

// First update the ship state, then if we are skimming move in the inward-spiraling
// square with the size change every spiral being 2 nm (1 nm per half spiral).
// This gives us a spiral with 1 nm between the course path and itself as it
// spirals inwards TODO clear up wording
void Skimmer::update() {
    Ship::update();
    if (!is_skimming())
        return;
    if (!can_move()) {
        reset_skimming_state();
        return;
    }

    // If we're still moving to our next corner, keep moving
    if (is_moving())
        return;

    // If we have finished our spiral, stop skimming.
    if (additional_sides_to_skim == 0) {
        cout << get_name() << " finished skimming spill" << endl;
        reset_skimming_state();
        return;
    }

    // Set the next direction that we will be moving on our spiral
    double next_direction;
    switch (skimming_state) {
    case SkimmingState_t::going_to_spill:
        skim_first_side();
        return;
    case SkimmingState_t::going_north:
        next_direction = 90.;
        skimming_state = SkimmingState_t::going_east;
        break;
    case SkimmingState_t::going_east:
        next_direction = 180.;
        skimming_state = SkimmingState_t::going_south;
        break;
    case SkimmingState_t::going_south:
        next_direction = 270.;
        skimming_state = SkimmingState_t::going_west;
        break;
    case SkimmingState_t::going_west:
        next_direction = 0.;
        skimming_state = SkimmingState_t::going_north;
        break;
    default:
        assert(0);
    }

    // Find the distance we need to travel for the next side
    int dist_to_travel = (additional_sides_to_skim + 3) / 2;
    --additional_sides_to_skim;

    // Calculate the new position we will be traveling to
    Point new_dest = get_location() + Compass_vector (next_direction, dist_to_travel);

    // Start moving to that new position at our max speed.
    set_destination_position_and_speed(new_dest, get_maximum_speed());
}

// Stop skimming
void Skimmer::stop() {
    reset_skimming_state();
    Ship::stop();
}

// Stop skimming if we are skimming.
void Skimmer::set_destination_position_and_speed(Point destination_position, double speed) {
    if (is_skimming())
        reset_skimming_state();
    Ship::set_destination_position_and_speed(destination_position, speed);
}

// Stop skimming if we are skimming.
void Skimmer::set_destination_island_and_speed(shared_ptr<Island> destination_island,
        double speed) {
    if (is_skimming())
        reset_skimming_state();
    Ship::set_destination_island_and_speed(destination_island, speed);
}

// Stop skimming if we are skimming.
void Skimmer::set_course_and_speed(double course, double speed) {
    if (is_skimming())
        reset_skimming_state();
    Ship::set_course_and_speed(course, speed);
}

// Make sure we can start skimming, then set up the skimming state
void Skimmer::start_skimming(Point spill_sw_corner_, int spill_size_) {
    if (!can_move())
        throw Error("Can't move!");
    if (spill_size_ <= 0)
        throw Error("Spill size can't be zero or negative!");

    // Set up the skimming state
    spill_sw_corner = spill_sw_corner_;
    spill_size = spill_size_;
    skimming_state = SkimmingState_t::going_to_spill;
    // Calculate the number of sides that we will have to travel in the spiral.
    // We will use this as a counter to keep track of where we are in the spiral
    // pattern and where we need to go to next
    additional_sides_to_skim = spill_size * 2 - 1;

    // Travel to the south west corner of the spill
    set_destination_position_and_speed(spill_sw_corner_, get_maximum_speed());
}

// Skim the first side of the spill, which has 1 nm less distance of travel than
// we would normally calculate based on the side number of sides left to skim.
void Skimmer::skim_first_side() {
    Point new_dest = get_location() + Compass_vector (0., spill_size);
    skimming_state = SkimmingState_t::going_north;
    --additional_sides_to_skim;
    set_destination_position_and_speed(new_dest, get_maximum_speed());
}

// Reset the state of the object
void Skimmer::reset_skimming_state() {
    skimming_state = SkimmingState_t::not_skimming;
    spill_size = 0;
    spill_sw_corner = {0., 0.};
}
