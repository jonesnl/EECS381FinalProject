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

// Construct a warshipo
Warship::Warship(const std::string& name_, Point position_, double fuel_capacity_,
        double maximum_speed, double fuel_consumption_, int resistance_, int firepower_,
        double attack_range_) :
            Ship(name_, position_, fuel_capacity_, maximum_speed, fuel_consumption_, resistance_),
            firepower(firepower_), attack_range(attack_range_), attacking(false) { }

// Uppdate the warship's state. If we are attacking a target, call receive_hit()
// on the target so that it takes the hit.
void Warship::update() {
    Ship::update();

    // Do nothing if not attacking
    if (!attacking)
        return;

    shared_ptr<Ship> target_ptr = target.lock();
    // If either us or the target is not afloat, stop attacking
    if (!is_afloat() || !target_ptr || !target_ptr->is_afloat()) {
        stop_attack();
        return;
    }

    cout << get_name() << " is attacking" << endl;
    double dist_to_target =
            cartesian_distance(get_location(), target_ptr->get_location());

    // If the target is in range, attack it, if not call the out of range handler function
    if (dist_to_target <= attack_range) {
        cout << get_name() << " fires" << endl;
        target_ptr->receive_hit(firepower, shared_from_this());
    } else {
        target_out_of_range_handler();
    }
}

// Describe if the warship is attacking anyone at the moment
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

// Move into the attacking state so on the next update it will attack the target
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

// Stop attacking the target
void Warship::stop_attack() {
    if (!attacking)
        throw Error("Was not attacking!");
    target.reset();
    attacking = false;
    cout << get_name() << " stopping attack" << endl;
}

/******** Cruiser Class ********/

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
void Cruiser::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr) {
    Warship::receive_hit(hit_force, attacker_ptr);
    if (is_afloat() && !is_attacking())
        attack(attacker_ptr);
}

// If we are out of range of our target, stop attacking
void Cruiser::target_out_of_range_handler() {
    cout << get_name() << " target is out of range" << endl;
    stop_attack();
}

/********** Torpedo_boat Class ********/

const double torpedo_boat_fuel_capacity_c = 800.;
const double torpedo_boat_maximum_speed_c = 12.;
const double torpedo_boat_fuel_consumption_c = 5.;
const int torpedo_boat_resistance_c = 9;
const int torpedo_boat_firepower_c = 3;
const double torpedo_boat_attack_range_c = 5.;
const double torpedo_boat_retreat_diest_c = 15.;

// Construct a Torpedo_boat
Torpedo_boat::Torpedo_boat(const string& name_, Point position_) :
        Warship(name_, position_, torpedo_boat_fuel_capacity_c,
                torpedo_boat_maximum_speed_c,
                torpedo_boat_fuel_consumption_c,
                torpedo_boat_resistance_c,
                torpedo_boat_firepower_c,
                torpedo_boat_attack_range_c) { }

// Add a ship type to the description
void Torpedo_boat::describe() const {
    cout << "\nTorpedo_boat ";
    Warship::describe();
}

// If we are hit, and we can move, take evasive action by retreating to the nearest
// island to the attacker that is at least torpedo_boat_retreat_dist_c away from the
// attacker. This boat does not counter attack.
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
                    return Compass_vector(attacker_position,
                            isl->get_location()).distance >= torpedo_boat_retreat_diest_c;
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

// If our target is out of range, and we can move, move towards the target's location
void Torpedo_boat::target_out_of_range_handler() {
    if (can_move())
        set_destination_position_and_speed(get_attack_target()->get_location(),
                get_maximum_speed());
}