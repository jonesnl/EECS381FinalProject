#include "Island.h"

#include "Model.h"

#include <iostream>

using namespace std;

/* Island class keeps track of data associated with an island. An island can
 * create and/or store oil at the island for ships to use to refuel.
 */

// Construct an island
Island::Island(const std::string &name_, Point position_, double fuel_,
        double production_rate_) : Sim_object(name_), position(position_),
        fuel(fuel_), production_rate(production_rate_) { }

// Update the island by producing some fuel
void Island::update() {
    // Production rate is assumed to be on a per hour basis according to the spec.
    if (production_rate > 0.)
        accept_fuel(production_rate);
}

// Describe the island
void Island::describe() const {
    cout << "\nIsland " << get_name() << " at position " << position << endl;
    cout << "Fuel available: " << fuel << " tons" << endl;
}

// Broadcast the location of the island if requested to
void Island::broadcast_current_state() const {
    Model::get_inst()->notify_location(get_name(), position);
}

// Provide some fuel we have in storage to a requesting ship
double Island::provide_fuel(double request) {
    if (request > fuel)
        request = fuel;
    fuel -= request;
    cout << "Island " << get_name() << " supplied " <<
            request << " tons of fuel" << endl;
    return request;
}

// Add some fuel to the island's fuel storage
void Island::accept_fuel(double amount) {
    fuel += amount;
    cout << "Island " << get_name() << " now has " << fuel << " tons" << endl;
}
