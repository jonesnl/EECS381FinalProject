#include "Cruiser.h"

#include <string>
#include <iostream>

using namespace std;

const double cruiser_fuel_capacity_c = 1000.;
const double cruiser_maximum_speed_c = 20.;
const double cruiser_fuel_consumption_c = 10.;
const int cruiser_resistance_c = 6;
const int cruiser_firepower_c = 3;
const double cruiser_attack_range_c = 15.;

// Construct a cruiser ship
Cruiser::Cruiser(const string &name_, Point position_) :
        Warship(name_, position_, cruiser_fuel_capacity_c, cruiser_maximum_speed_c,
                cruiser_fuel_consumption_c, cruiser_resistance_c,
                cruiser_firepower_c, cruiser_attack_range_c) { }

// Add a ship type to the description
void Cruiser::describe() const {
    cout << "\nCruiser ";
    Warship::describe();
}

// If we are hit, counter attack if we are not already attacking
void Cruiser::receive_hit(int hit_force, shared_ptr<Ship_component> attacker_ptr) {
    Warship::receive_hit(hit_force, attacker_ptr);
    if (is_afloat() && !is_attacking())
        attack(attacker_ptr);
}

// If we are out of range of our target, stop attacking
void Cruiser::target_out_of_range_handler() {
    cout << get_name() << " target is out of range" << endl;
    stop_attack();
}
