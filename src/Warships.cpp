#include "Warships.h"

#include "Utility.h"
#include "Model.h"
#include "Island.h"

#include <iostream>
#include <cassert>
#include <algorithm>

using namespace std;

/* Warships are ships that can attack other ships. The file contains three classes:
 * Warship, Cruiser, and Torpedo_boat.
 */

/********* Warship Class ***********/

Warship::Warship(const std::string& name_, Point position_, double fuel_capacity_,
        double maximum_speed, double fuel_consumption_, int resistance_, int firepower_,
        double attack_range_) :
            Ship(name_, position_, fuel_capacity_, maximum_speed, fuel_consumption_, resistance_),
            firepower(firepower_), attack_range(attack_range_) { }

void Warship::update() {
    Ship::update();
    if (!attacking)
        return;
    shared_ptr<Ship> target_ptr = target.lock();
    if (!is_afloat() || !target_ptr || !target_ptr->is_afloat()) {
        stop_attack();
        return;
    }
    cout << get_name() << " is attacking" << endl;
    double dist_to_target =
            cartesian_distance(get_location(), target_ptr->get_location());
    if (dist_to_target <= attack_range) {
        cout << get_name() << " fires" << endl;
        target_ptr->receive_hit(firepower, shared_from_this());
    } else {
        target_out_of_range_handler();
    }
}

void Warship::describe() const {
    Ship::describe();
    if (attacking) {
        shared_ptr<Ship> target_ptr = target.lock();
        if (!target_ptr || !target_ptr->is_afloat()) {
            cout << "Attacking absent ship" << endl;
        } else {
            cout << "Attacking " << target_ptr->get_name() << endl;
        }
    }
}

void Warship::attack(std::shared_ptr<Ship> target_ptr_) {
    assert(target_ptr_);
    if (!is_afloat())
        throw Error("Cannot attack!");
    if (target_ptr_ == shared_from_this())
        throw Error("Cannot attack itself!");
    if (target_ptr_ == target.lock())
        throw Error("Already attacking this target!");
    target = target_ptr_;
    attacking = true;
    cout << get_name() << " will attack " << target_ptr_->get_name() << endl;
}

void Warship::stop_attack() {
    if (!attacking)
        throw Error("Was not attacking!");
    target.reset();
    attacking = false;
    cout << get_name() << " stopping attack" << endl;
}

/******** Cruiser Class ********/

Cruiser::Cruiser(const string &name_, Point position_) :
        Warship(name_, position_, 1000., 20., 10., 6, 3, 15.) { }

void Cruiser::describe() const {
    cout << "\nCruiser ";
    Warship::describe();
}

void Cruiser::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr) {
    Warship::receive_hit(hit_force, attacker_ptr);
    if (is_afloat() && !is_attacking())
        attack(attacker_ptr);
}

void Cruiser::target_out_of_range_handler() {
    cout << get_name() << " target is out of range" << endl;
    stop_attack();
}

/********** Torpedo_boat Class ********/

const double torpedo_boat_fuel_capacity_c = 800.;
const double torpedo_boat_maximum_speed_c = 12.;
const double torpedo_boat_fuel_consumption_c = 5;
const int torpedo_boat_firepower_c = 3;
const double torpedo_boat_attack_range_c = 5;
const int torpedo_boat_resistance_c = 9;

Torpedo_boat::Torpedo_boat(const string& name_, Point position_) :
        Warship(name_, position_, torpedo_boat_fuel_capacity_c,
                torpedo_boat_maximum_speed_c,
                torpedo_boat_fuel_consumption_c,
                torpedo_boat_resistance_c,
                torpedo_boat_firepower_c,
                torpedo_boat_attack_range_c) { }

void Torpedo_boat::describe() const {
    cout << "\nTorpedo_boat ";
    Warship::describe();
}

void Torpedo_boat::receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr) {
    Warship::receive_hit(hit_force, attacker_ptr);
    if (can_move()) {
        cout << get_name() << " taking evasive action" << endl;
        if (is_attacking())
            stop_attack();
        // Take evasive action

        // Vector is sorted by island name by default
        auto island_vect = Model::get_inst()->get_vector_of_islands();
        assert(island_vect.size() > 0);

        Point attacker_position = attacker_ptr->get_location();
        // Sort by distance from the attacker, closest first
        stable_sort(island_vect.begin(), island_vect.end(),
                IslandDistComp{attacker_position});

        // Find closest island that is at least 15 nm from the attacker
        auto dest_itt = find_if(island_vect.begin(), island_vect.end(),
                [attacker_position](shared_ptr<Island> isl) {
                    return Compass_vector(attacker_position, isl->get_location()).distance >= 15.;
                });
        // If no island at least 15 nm from attacker, choose island that is
        // furthest from the attacker
        if (dest_itt == island_vect.end()) {
            // Can't just take the last element of the array since we still
            // need to take the name into account if the last couple islands
            // are the same distance from each other.
            dest_itt = max_element(island_vect.begin(), island_vect.end(),
                    IslandDistComp{attacker_position});
        }
        assert(dest_itt != island_vect.end());
        set_destination_island_and_speed(*dest_itt, get_maximum_speed());
    }
}

void Torpedo_boat::target_out_of_range_handler() {
    if (can_move())
        set_destination_position_and_speed(get_attack_target()->get_location(),
                get_maximum_speed());
}