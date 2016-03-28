#include "Cruise_ship.h"
#include "Model.h"
#include "Island.h"

#include <limits>
#include <algorithm>
#include <iostream>
#include <cassert>

using namespace std;

Cruise_ship::Cruise_ship(const std::string &name_, Point position_) :
        Ship(name_, position_, 500, 15, 2, 0),
        cruise_state(CruiseState_t::not_cruising) { }

void Cruise_ship::update() {
    Ship::update();
    if (!can_move())
        cancel_cruise();

    switch(cruise_state) {
    case CruiseState_t::not_cruising:
        break;
    case CruiseState_t::cruising:
        if (can_dock(cruise_next_destination)) {
            dock(cruise_next_destination);
            if (cruise_next_destination == origin_island && islands_to_visit.empty()) {
                cout << get_name() << " cruise is over at " <<
                        origin_island->get_name() << endl;
                clear_cruise_data();
            } else
                cruise_state = CruiseState_t::arriving;
        }
        break;
    case CruiseState_t::arriving:
        refuel();
        cruise_state = CruiseState_t::loading_unloading;
        break;
    case CruiseState_t::loading_unloading:
        cruise_state = CruiseState_t::departing;
        break;
    case CruiseState_t::departing:
        cruise_to(next_island_to_visit());
        break;
    }

}

void Cruise_ship::describe() const {
    cout << "\nCruise_ship ";
    Ship::describe();
    switch (cruise_state) {
    case CruiseState_t::not_cruising:
        break;
    case CruiseState_t::cruising:
        cout << "On cruise to " << get_destination_Island()->get_name() << endl;
        break;
    case CruiseState_t::arriving:
    case CruiseState_t::loading_unloading:
    case CruiseState_t::departing:
        cout << "Waiting during cruise at " <<
                get_docked_Island()->get_name() << endl;
        break;
    }
}

void Cruise_ship::stop() {
    cancel_cruise();
    Ship::stop();
}

void Cruise_ship::set_destination_position_and_speed(Point destination_position,
        double speed) {
    cancel_cruise();
    Ship::set_destination_position_and_speed(destination_position, speed);
}

void Cruise_ship::set_destination_island_and_speed(
        std::shared_ptr<Island> destination_island, double speed) {
    cancel_cruise();
    origin_island = destination_island; // TODO if error, these are set when they shouldn't be
    cruise_speed = speed;
    cruise_to(destination_island);
    auto island_vect = Model::get_inst()->get_vector_of_islands();
    islands_to_visit =
            set<shared_ptr<Island>, IslandNameComp>(island_vect.begin(), island_vect.end());
    islands_to_visit.erase(origin_island);
    cout << get_name() << " cruise will start and end at " <<
            destination_island->get_name() << endl;
}

void Cruise_ship::set_course_and_speed(double course, double speed) {
    cancel_cruise();
    Ship::set_course_and_speed(course, speed);
}

/* Private helper functions */

void Cruise_ship::cruise_to(shared_ptr<Island> island) {
    Ship::set_destination_island_and_speed(island, cruise_speed);
    cout << get_name() << " will visit " << island->get_name() << endl;
    cruise_next_destination = island;
    cruise_state = CruiseState_t::cruising;
}

shared_ptr<Island> Cruise_ship::next_island_to_visit() {
    assert(is_docked());
    if (islands_to_visit.empty())
        return origin_island;

    auto next_island_itt = min_element(islands_to_visit.cbegin(),
            islands_to_visit.cend(), IslandDistComp{get_docked_Island()->get_location()});
    shared_ptr<Island> next_island_ptr = *next_island_itt;
    islands_to_visit.erase(next_island_itt);
    return next_island_ptr;
}

void Cruise_ship::cancel_cruise() {
    if (cruise_state == CruiseState_t::not_cruising)
        return;
    clear_cruise_data();
    cout << get_name() << " canceling current cruise" << endl;
}

void Cruise_ship::clear_cruise_data() {
    cruise_state = CruiseState_t::not_cruising;
    origin_island = nullptr;
    cruise_speed = 0.0;
    islands_to_visit.clear();
}
