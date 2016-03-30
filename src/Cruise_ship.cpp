#include "Cruise_ship.h"
#include "Model.h"
#include "Island.h"

#include <limits>
#include <algorithm>
#include <iostream>
#include <cassert>

using namespace std;

const double cruise_ship_fuel_capacity_c = 500.;
const double cruise_ship_maximum_speed_c = 15.;
const double cruise_ship_fuel_consumption_c = 2;
const int cruise_ship_resistance_c = 0;

/* Public Function Definitions */

// Constructor for the cruise ship
Cruise_ship::Cruise_ship(const std::string &name_, Point position_) :
        Ship(name_, position_, cruise_ship_fuel_capacity_c,
                cruise_ship_maximum_speed_c, cruise_ship_fuel_consumption_c,
                cruise_ship_resistance_c),
        cruise_state(CruiseState_t::not_cruising), cruise_speed(0.) { }

// Updates the cruise ship according the project spec.
void Cruise_ship::update() {
    Ship::update();

    if (!can_move())
        cancel_cruise();

    switch(cruise_state) {
    case CruiseState_t::not_cruising:
        break;
    case CruiseState_t::cruising:
        // If we can dock at our next destination, do so. If we're finished,
        // let the user know.
        if (can_dock(next_destination)) {
            dock(next_destination);
            if (next_destination == origin_island && islands_to_visit.empty()) {
                cout << get_name() << " cruise is over at " <<
                        origin_island->get_name() << endl;
                clear_cruise_data();
            } else
                cruise_state = CruiseState_t::arriving;
        }
        break;
    case CruiseState_t::arriving:
        // First cycle we're at an island on a cruise, refuel
        refuel();
        cruise_state = CruiseState_t::loading_unloading;
        break;
    case CruiseState_t::loading_unloading:
        // Second cycle let the passengers explore the island
        cruise_state = CruiseState_t::departing;
        break;
    case CruiseState_t::departing:
        // Third cycle, start the cruise to the next island
        cruise_to(next_island_to_visit());
        break;
    }
}

// Describe the cruse ship, and whether we're on a cruise or not.
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

// Cancel the cruise if we're told to stop
void Cruise_ship::stop() {
    cancel_cruise();
    Ship::stop();
}

// Cancel the cruise if we set a new destination position
void Cruise_ship::set_destination_position_and_speed(Point destination_position,
        double speed) {
    cancel_cruise();
    Ship::set_destination_position_and_speed(destination_position, speed);
}

// Cancel the cruise if we set new destination, then start a new cruise starting
// at destination_island.
void Cruise_ship::set_destination_island_and_speed(
        std::shared_ptr<Island> destination_island, double speed) {
    cancel_cruise();

    // Set the island where the cruise is starting at
    origin_island = destination_island;
    // Set the speed of the cruise
    cruise_speed = speed;
    // Start cruising to the origin island
    cruise_to(origin_island);

    // Populate the set of islands that we will visit on the cruise
    auto island_vect = Model::get_inst()->get_vector_of_islands();
    islands_to_visit = {island_vect.begin(), island_vect.end()};
    // Remove the origin island so it is not included until the very end of the
    // cruise. We handle that case specially.
    islands_to_visit.erase(origin_island);

    cout << get_name() << " cruise will start and end at " <<
            origin_island->get_name() << endl;
}

// Cancel the cruise if we set a new course for the ship
void Cruise_ship::set_course_and_speed(double course, double speed) {
    cancel_cruise();
    Ship::set_course_and_speed(course, speed);
}

/* Private helper functions */

// Start cruising towards next_island
void Cruise_ship::cruise_to(shared_ptr<Island> next_island) {
    Ship::set_destination_island_and_speed(next_island, cruise_speed);
    cout << get_name() << " will visit " << next_island->get_name() << endl;
    next_destination = next_island;
    cruise_state = CruiseState_t::cruising;
}

// Calculate what the next island we should visit on the cruise is
shared_ptr<Island> Cruise_ship::next_island_to_visit() {
    assert(is_docked());
    // If we have visited all islands, then return to the island where the cruise
    // started at
    if (islands_to_visit.empty())
        return origin_island;

    // Use the island that is closest to us that we haven't visited yet as the
    // next island in our cruise. Erase it from the set of islands we still need
    // to visit.
    auto next_island_itt = min_element(islands_to_visit.cbegin(),
            islands_to_visit.cend(), IslandDistComp{get_docked_Island()->get_location()});
    shared_ptr<Island> next_island_ptr = *next_island_itt;
    islands_to_visit.erase(next_island_itt);
    return next_island_ptr;
}

// Cancel a cruise if we are cruising
void Cruise_ship::cancel_cruise() {
    if (cruise_state == CruiseState_t::not_cruising)
        return;
    clear_cruise_data();
    cout << get_name() << " canceling current cruise" << endl;
}

// Clear data associated with a cruise. Used at the end of a cruise or if a cruise
// is canceled
void Cruise_ship::clear_cruise_data() {
    cruise_state = CruiseState_t::not_cruising;
    origin_island = nullptr;
    cruise_speed = 0.0;
    islands_to_visit.clear();
}
