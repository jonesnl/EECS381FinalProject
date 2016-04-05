#include "Tanker.h"
#include "Utility.h"
#include "Island.h"

#include <iostream>
#include <map>

using namespace std;

const double tanker_fuel_capacity_c = 100.;
const double tanker_maximum_speed_c = 10.;
const double tanker_fuel_consumption_c = 2.;
const int tanker_resistance_c = 0;
const double tanker_cargo_capacity_c = 1000.;

/* Public member functions */

// Construct a new tanker
Tanker::Tanker(const string &name_, Point position_) :
        Ship(name_, position_, tanker_fuel_capacity_c, tanker_maximum_speed_c,
                tanker_fuel_consumption_c, tanker_resistance_c),
        tanker_state(TankerState_t::no_cargo_dest), cargo(0.) { }

// Set destination as long as we aren't hauling cargo
void Tanker::set_destination_position_and_speed(Point destination_point,
        double speed) {
    not_hauling_cargo_or_error();
    Ship::set_destination_position_and_speed(destination_point, speed);
}

// Set destination as long as we aren't hauling cargo
void Tanker::set_destination_island_and_speed(shared_ptr<Island> destination_island,
        double speed) {
    not_hauling_cargo_or_error();
    Ship::set_destination_island_and_speed(destination_island, speed);
}

// Set course as long as we aren't hauling cargo
void Tanker::set_course_and_speed(double course, double speed) {
    not_hauling_cargo_or_error();
    Ship::set_course_and_speed(course, speed);
}

// Set the island where we will load cargo. Fails if we are already hauling cargo.
// If both loading and unloading islands are set and are not the same, start hauling cargo
// between the two
void Tanker::set_load_destination(shared_ptr<Island> island) {
    not_hauling_cargo_or_error();
    loading_island = island;
    if (island == unloading_island)
        throw Error("Load and unload cargo destinations are the same!");

    cout << get_name() << " will load at " << island->get_name() << endl;
    start_tanker_cycle_if_possible();
}

// Set the island where we will unload cargo. Fails if we are already hauling cargo.
// If both loading and unloading islands are set and are not the same, start hauling cargo
// between the two
void Tanker::set_unload_destination(shared_ptr<Island> island) {
    not_hauling_cargo_or_error();
    unloading_island = island;
    if (island == loading_island)
        throw Error("Load and unload cargo destinations are the same!");

    cout << get_name() << " will unload at " << island->get_name() << endl;
    start_tanker_cycle_if_possible();
}

// Stop hauling cargo if we are doing so.
void Tanker::stop() {
    Ship::stop();
    stop_tanker_cycle();
}

// Simulate a single timestep of cargo hauling
void Tanker::update() {
    Ship::update();
    if (!can_move()) {
        stop_tanker_cycle();
        return;
    }

    // Used in loading state below to hold how much fuel we need to fill the cargo bay.
    double fuel_needed_to_fill_cargo;

    switch (tanker_state) {
    case TankerState_t::no_cargo_dest:
        break;
    case TankerState_t::moving_to_loading:
        if (can_dock(loading_island)) {
            dock(loading_island);
            tanker_state = TankerState_t::loading;
        }
        break;
    case TankerState_t::moving_to_unloading:
        if (can_dock(unloading_island)) {
            dock(unloading_island);
            tanker_state = TankerState_t::unloading;
        }
        break;
    case TankerState_t::loading:
        // When loading cargo, refuel
        refuel();
        fuel_needed_to_fill_cargo = tanker_cargo_capacity_c - cargo;
        if (fuel_needed_to_fill_cargo < double_close_enough_c) {
            // If our fuel level is close enough to full, set it to full and go
            // start going to unload the cargo.
            cargo = tanker_cargo_capacity_c;
            Ship::set_destination_island_and_speed(unloading_island,
                    get_maximum_speed());
            tanker_state = TankerState_t::moving_to_unloading;
        } else {
            // Continue to load cargo at the island until the cargo bay is full
            cargo += loading_island->provide_fuel(fuel_needed_to_fill_cargo);
            cout << get_name() << " now has " << cargo << " of cargo" << endl;
        }
        break;
    case TankerState_t::unloading:
        if (cargo == 0.) {
            // Start moving to load more cargo at the loading_island
            Ship::set_destination_island_and_speed(loading_island,
                    get_maximum_speed());
            tanker_state = TankerState_t::moving_to_loading;
        } else {
            // Unload the cargo
            unloading_island->accept_fuel(cargo);
            cargo = 0.;
        }
        break;
    }
}

// Describe the state of the tanker
void Tanker::describe() const {
    static map<TankerState_t, string> describe_map = {
            {TankerState_t::no_cargo_dest, ", no cargo destinations"},
            {TankerState_t::loading, ", loading"},
            {TankerState_t::unloading, ", unloading"},
            {TankerState_t::moving_to_loading, ", moving to loading destination"},
            {TankerState_t::moving_to_unloading, ", moving to unloading destination"}
    };
    cout << "\nTanker ";
    Ship::describe();
    cout << "Cargo: " << cargo << " tons" << describe_map[tanker_state] << endl;
}

// Provide fuel
double Tanker::provide_fuel(double request) {
    double return_fuel;
    if (request < cargo) {
        return_fuel = request;
        cargo -= request;
    } else {
        return_fuel = cargo;
        cargo = 0.;
    }
    return return_fuel;
}

/* Private member functions */

// Throw an error if we are hauling cargo
void Tanker::not_hauling_cargo_or_error() {
    if (tanker_state != TankerState_t::no_cargo_dest)
        throw Error("Tanker has cargo destinations!");
}

// If we can, start hauling cargo between the loading and unloading islands.
// Which state we start in depends on the current state of the tanker
void Tanker::start_tanker_cycle_if_possible() {
    if (!loading_island || !unloading_island)
        return;
    if (is_docked()) {
        if (get_docked_Island() == loading_island) {
            // If we are docked at the loading island, start loading
            tanker_state = TankerState_t::loading;
            return;
        }
        if (get_docked_Island() == unloading_island) {
            // If we are docked at the unloading island, start unloading
            tanker_state = TankerState_t::unloading;
            return;
        }
    }
    if (!is_moving()) {
        if (cargo == 0. && can_dock(loading_island)) {
            // If we can dock at the loading island and we have no cargo, do so to load some
            dock(loading_island);
            tanker_state = TankerState_t::loading;
            return;
        }
        if (cargo > 0. && can_dock(unloading_island)) {
            // If we can dock at the unloading island and we don't have any cargo,
            // do so to unload it
            dock(unloading_island);
            tanker_state = TankerState_t::unloading;
            return;
        }
    }
    if (cargo == 0.) {
        // If we have no cargo, move to load some
        Ship::set_destination_island_and_speed(loading_island, get_maximum_speed());
        tanker_state = TankerState_t::moving_to_loading;
        return;
    }
    if (cargo > 0.) {
        // If we have some cargo, move to unload it
        Ship::set_destination_island_and_speed(unloading_island, get_maximum_speed());
        tanker_state = TankerState_t::moving_to_unloading;
        return;
    }
}

// Stop the tanker cycle and reset internal state
void Tanker::stop_tanker_cycle() {
    loading_island.reset();
    unloading_island.reset();
    tanker_state = TankerState_t::no_cargo_dest;
    cout << get_name() << " now has no cargo destinations" << endl;
}