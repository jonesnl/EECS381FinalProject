#include "Skimmer.h"
#include "Utility.h"

#include <iostream>
#include <cassert>

using namespace std;

const double skimmer_fuel_capacity_c = 100.;
const double skimmer_maximum_speed_c = 2.;
const double skimmer_fuel_consumption_c = 0.5;
const int skimmer_resistance_c = 0;

Skimmer::Skimmer(const string &name_, Point position_) :
        Ship(name_, position_, skimmer_fuel_capacity_c,
                skimmer_maximum_speed_c, skimmer_fuel_consumption_c,
                skimmer_resistance_c),
        skimming_state(SkimmingState_t::not_skimming) { }

void Skimmer::describe() const {
    cout << "\nSkimmer ";
    Ship::describe();
    if (is_skimming())
        cout << "Is skimming a spill of size " << spill_size <<
                " starting at " << spill_ll_corner << endl;
}

// TODO add destination point stuff and fix logic
void Skimmer::update() {
    Ship::update();
    if (!is_skimming())
        return;
    if (!can_move()) {
        stop_skimming();
        return;
    }

    // If we're still moving to our next point, keep moving
    if (is_moving())
        return;

    if (additional_sides_to_skim == 0) {
        cout << get_name() << " finished skimming spill" << endl;
        stop_skimming();
        return;
    }

    double next_direction;
    switch (skimming_state) {
    case SkimmingState_t::going_to_spill:
        skim_first_side();
        return;
    case SkimmingState_t::going_up:
        next_direction = 90.;
        skimming_state = SkimmingState_t::going_right;
        break;
    case SkimmingState_t::going_right:
        next_direction = 180.;
        skimming_state = SkimmingState_t::going_down;
        break;
    case SkimmingState_t::going_down:
        next_direction = 270.;
        skimming_state = SkimmingState_t::going_left;
        break;
    case SkimmingState_t::going_left:
        next_direction = 0.;
        skimming_state = SkimmingState_t::going_up;
        break;
    default:
        assert(0);
    }

    int dist_to_travel = (additional_sides_to_skim + 3) / 2;
    --additional_sides_to_skim;
    Point new_dest = get_location() + Compass_vector (next_direction, dist_to_travel);

    set_destination_position_and_speed(new_dest, get_maximum_speed());
}

void Skimmer::start_skimming(Point spill_ll_corner_, int spill_size_) {
    if (!can_move())
        throw Error("Can't move!");
    if (spill_size_ <= 0)
        throw Error("Spill size can't be zero or negative!");

    spill_ll_corner = spill_ll_corner_;
    spill_size = spill_size_;
    additional_sides_to_skim = spill_size * 2 - 1;
    skimming_state = SkimmingState_t::going_to_spill;
    set_destination_position_and_speed(spill_ll_corner_, get_maximum_speed());
}

void Skimmer::stop() {
    Ship::stop();
    stop_skimming();
}

void Skimmer::skim_first_side() {
    Point new_dest = get_location() + Compass_vector (0., spill_size);
    skimming_state = SkimmingState_t::going_up;
    --additional_sides_to_skim;
    set_destination_position_and_speed(new_dest, get_maximum_speed());
}

void Skimmer::stop_skimming() {
    skimming_state = SkimmingState_t::not_skimming;
    spill_size = 0;
    spill_ll_corner = {0., 0.};
}
