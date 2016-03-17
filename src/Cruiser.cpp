#include "Cruiser.h"
#include "Utility.h"

#include <iostream>
#include <cassert>

using namespace std;

Cruiser::Cruiser(const std::string &name_, Point position_) :
        Ship(name_, position_, 1000., 20., 10., 6) {
    cout << "Cruiser " << get_name() << " constructed" << endl;
}

Cruiser::~Cruiser() {
    cout << "Cruiser " << get_name() << " destructed" << endl;
}

void Cruiser::update() {
    Ship::update();
    if (!target)
        return;
    if (!is_afloat() || !target->is_afloat()) {
        stop_attack();
        return;
    }
    cout << get_name() << " is attacking" << endl;
    double dist_to_target =
            cartesian_distance(get_location(), target->get_location());
    if (dist_to_target <= attack_range) {
        cout << get_name() << " fires" << endl;
        target->receive_hit(firepower, this);
    } else {
        cout << get_name() << " target is out of range" << endl;
        stop_attack();
    }
}

void Cruiser::describe() const {
    cout << "\nCruiser ";
    Ship::describe();
    if (target)
        cout << "Attacking " << target->get_name() << endl;
}

void Cruiser::receive_hit(int hit_force, Ship *attacker_ptr) {
    Ship::receive_hit(hit_force, attacker_ptr);
    if (!target)
        attack(attacker_ptr);
}

void Cruiser::attack(Ship *target_ptr_) {
    assert(target_ptr_);
    if (!is_afloat())
        throw Error("Cannot attack!");
    if (target_ptr_ == this)
        throw Error("Cannot attack itself!");
    if (target_ptr_ == target)
        throw Error("Already attacking this target!");
    target = target_ptr_;
    cout << get_name() << " will attack " << target_ptr_->get_name() << endl;
}

void Cruiser::stop_attack() {
    if (!target)
        throw Error("Was not attacking!");
    target = nullptr;
    cout << get_name() << " stopping attack" << endl;
}