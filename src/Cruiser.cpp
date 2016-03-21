#include "Cruiser.h"
#include "Utility.h"

#include <iostream>
#include <cassert>

using namespace std;

Cruiser::Cruiser(const string &name_, Point position_) :
        Ship(name_, position_, 1000., 20., 10., 6) {
    cout << "Cruiser " << get_name() << " constructed" << endl;
}

Cruiser::~Cruiser() {
    cout << "Cruiser " << get_name() << " destructed" << endl;
}

void Cruiser::update() {
    Ship::update();
    if (!attacking)
        return;
    if (target.expired()) {
        stop_attack();
        return;
    }
    shared_ptr<Ship> target_ptr = target.lock();
    if (!is_afloat() || !target_ptr->is_afloat()) {
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
        cout << get_name() << " target is out of range" << endl;
        stop_attack();
    }
}

void Cruiser::describe() const {
    cout << "\nCruiser ";
    Ship::describe();
    if (attacking) {
        shared_ptr<Ship> target_ptr = target.lock();
        if (!target_ptr || !target_ptr->is_afloat()) {
            cout << "Attacking absent ship" << endl;
        } else {
            cout << "Attacking " << target.lock()->get_name() << endl;
        }
    }
}

void Cruiser::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr) {
    Ship::receive_hit(hit_force, attacker_ptr);
    if (is_afloat() && !attacking)
        attack(attacker_ptr);
}

void Cruiser::attack(shared_ptr<Ship> target_ptr_) {
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

void Cruiser::stop_attack() {
    if (!attacking)
        throw Error("Was not attacking!");
    target.reset();
    attacking = false;
    cout << get_name() << " stopping attack" << endl;
}