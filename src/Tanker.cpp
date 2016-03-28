#include "Tanker.h"
#include "Utility.h"
#include "Island.h"

#include <iostream>
#include <map>

using namespace std;

Tanker::Tanker(const string &name_, Point position_) :
        Ship(name_, position_, 100, 10, 2., 0),
        tanker_state(TankerState_t::no_cargo_dest), cargo(0.),
        cargo_capacity(1000.) { }

void Tanker::set_destination_position_and_speed(Point destination_point,
        double speed) {
    no_cargo_dest_or_error();
    Ship::set_destination_position_and_speed(destination_point, speed);
}

void Tanker::set_destination_island_and_speed(shared_ptr<Island> destination_island,
        double speed) {
    no_cargo_dest_or_error();
    Ship::set_destination_island_and_speed(destination_island, speed);
}

void Tanker::set_course_and_speed(double course, double speed) {
    no_cargo_dest_or_error();
    Ship::set_course_and_speed(course, speed);
}

void Tanker::set_load_destination(shared_ptr<Island> island) {
    no_cargo_dest_or_error();
    loading_island = island;
    if (island == unloading_island.lock())
        throw Error("Load and unload cargo destinations are the same!");

    cout << get_name() << " will load at " << island->get_name() << endl;
    start_tanker_cycle_if_possible();
}

void Tanker::set_unload_destination(shared_ptr<Island> island) {
    no_cargo_dest_or_error();
    unloading_island = island;
    if (island == loading_island.lock())
        throw Error("Load and unload cargo destinations are the same!");

    cout << get_name() << " will unload at " << island->get_name() << endl;
    start_tanker_cycle_if_possible();
}

void Tanker::stop() {
    Ship::stop();
    stop_cargo_loop();
}

void Tanker::update() {
    Ship::update();
    if (!can_move()) {
        stop_cargo_loop();
        return;
    }
    double fuel_needed_to_fill_cargo; // TODO refactor?
    shared_ptr<Island> locked_loading_island = loading_island.lock();
    shared_ptr<Island> locked_unloading_island = unloading_island.lock();
    switch (tanker_state) {
    case TankerState_t::no_cargo_dest:
        break;
    case TankerState_t::moving_to_loading:
        if (can_dock(locked_loading_island)) {
            dock(locked_loading_island);
            tanker_state = TankerState_t::loading;
        }
        break;
    case TankerState_t::moving_to_unloading:
        if (can_dock(locked_unloading_island)) {
            dock(locked_unloading_island);
            tanker_state = TankerState_t::unloading;
        }
        break;
    case TankerState_t::loading:
        refuel();
        fuel_needed_to_fill_cargo = cargo_capacity - cargo;
        if (fuel_needed_to_fill_cargo < double_close_enough_c) {
            cargo = cargo_capacity;
            Ship::set_destination_island_and_speed(locked_unloading_island,
                    get_maximum_speed());
            tanker_state = TankerState_t::moving_to_unloading;
        } else {
            cargo += locked_loading_island->provide_fuel(fuel_needed_to_fill_cargo);
            cout << get_name() << " now has " << cargo << " of cargo" << endl;
        }
        break;
    case TankerState_t::unloading:
        if (cargo == 0.) {
            Ship::set_destination_island_and_speed(locked_loading_island,
                    get_maximum_speed());
            tanker_state = TankerState_t::moving_to_loading;
        } else {
            locked_unloading_island->accept_fuel(cargo);
            cargo = 0.;
        }
        break;
    }
}

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

void Tanker::no_cargo_dest_or_error() {
    if (tanker_state != TankerState_t::no_cargo_dest)
        throw Error("Tanker has cargo destinations!");
}

void Tanker::start_tanker_cycle_if_possible() {
    if (loading_island.expired() || unloading_island.expired())
        return;
    shared_ptr<Island> locked_loading_island = loading_island.lock();
    shared_ptr<Island> locked_unloading_island = unloading_island.lock();
    if (is_docked()) {
        if (get_docked_Island() == loading_island.lock()) {
            tanker_state = TankerState_t::loading;
            return;
        }
        if (get_docked_Island() == unloading_island.lock()) {
            tanker_state = TankerState_t::unloading;
            return;
        }
    }
    if (!is_moving()) {
        if (cargo == 0. && can_dock(locked_loading_island)) {
            dock(locked_loading_island);
            tanker_state = TankerState_t::loading;
            return;
        }
        if (cargo > 0. && can_dock(locked_unloading_island)) {
            dock(locked_unloading_island);
            tanker_state = TankerState_t::unloading;
            return;
        }
    }
    if (cargo == 0.) {
        Ship::set_destination_island_and_speed(locked_loading_island, get_maximum_speed());
        tanker_state = TankerState_t::moving_to_loading;
        return;
    }
    if (cargo > 0.) {
        Ship::set_destination_island_and_speed(locked_unloading_island, get_maximum_speed());
        tanker_state = TankerState_t::moving_to_unloading;
        return;
    }
}

void Tanker::stop_cargo_loop() {
    loading_island.reset();
    unloading_island.reset();
    tanker_state = TankerState_t::no_cargo_dest;
    cout << get_name() << " now has no cargo destinations" << endl;
}