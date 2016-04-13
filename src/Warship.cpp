#include "Warship.h"

#include "Utility.h"
#include "Model.h"

#include <iostream>
#include <cassert>
#include <algorithm>

using namespace std;

/* Warships are ships that can attack other ships. The file contains three classes:
 * Warship, Cruiser, and Torpedo_boat.
 */

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
void Warship::attack(std::shared_ptr<Ship_component> target_ptr_) {
    assert(target_ptr_);
    if (!is_afloat())
        throw Error("Cannot attack!");
    auto target_ship_ptr = dynamic_pointer_cast<Ship>(target_ptr_);
    if (!target_ship_ptr)
        throw Error("Target is not a ship!");
    if (target_ship_ptr == shared_from_this())
        throw Error("Cannot attack itself!");
    if (target_ship_ptr == target.lock())
        throw Error("Already attacking this target!");
    target = target_ship_ptr;
    attacking = true;
    cout << get_name() << " will attack " << target_ship_ptr->get_name() << endl;
}

// Stop attacking the target
void Warship::stop_attack() {
    if (!attacking)
        throw Error("Was not attacking!");
    target.reset();
    attacking = false;
    cout << get_name() << " stopping attack" << endl;
}

