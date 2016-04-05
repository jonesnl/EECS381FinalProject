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
        cout << "Is skimming" << endl;
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
    double next_side_dist = skimmed_size;
    double next_side_direction;

    if (skimming_state == SkimmingState_t::going_to_spill &&
            get_location() == spill_origin) {
        skimmed_size = 0.;
        skimming_state = SkimmingState_t::going_up;
        next_side_direction = 0.;
        next_side_dist += 1;
    } else {
        switch (skimming_state) {
        case SkimmingState_t::going_up:
            skimming_state = SkimmingState_t::going_right;
            next_side_direction = 90.;
            skimmed_size += 1;
            next_side_dist += 1;
            break;
        case SkimmingState_t::going_right:
            skimming_state = SkimmingState_t::going_down;
            next_side_direction = 180.;
            break;
        case SkimmingState_t::going_down:
            skimming_state = SkimmingState_t::going_left;
            next_side_direction = 270.;
            skimmed_size += 1;
            next_side_dist += 1;
            break;
        case SkimmingState_t::going_left:
            skimming_state = SkimmingState_t::going_up;
            next_side_direction = 0.;
            skimmed_size += 1.;
            break;
        default:
            assert(0);
        }
    }
    Point next_dest = get_location() + Compass_vector{next_side_direction, next_side_dist};

    set_destination_position_and_speed(next_dest, get_maximum_speed());
}

void Skimmer::start_skimming(Point spill_origin_, double spill_size_) {
    if (!can_move())
        throw Error("Can't move!");

    spill_origin = spill_origin_;
    spill_size = spill_size_;
    skimming_state = SkimmingState_t::going_to_spill;
    set_destination_position_and_speed(spill_origin_, get_maximum_speed());
}

void Skimmer::stop() {
    Ship::stop();
    stop_skimming();
}

void Skimmer::stop_skimming() {
    skimming_state = SkimmingState_t::not_skimming;
    skimmed_size = 0.;
    spill_size = 0.;
    spill_origin = {0., 0.};
}
