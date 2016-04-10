#include <algorithm>
#include <iostream>
#include "Ship_composite.h"
#include "Utility.h"

using namespace std;
using namespace placeholders;

Ship_composite::Ship_composite(const string& name_) :
        Ship_component(name_) { }

Ship_composite::~Ship_composite() {
    for (auto& child : children) {
        child.second.lock()->remove_parent();
    }
}

void Ship_composite::add_component(shared_ptr<Ship_component> ship_ptr) {
    ship_ptr->add_parent(shared_from_this());
    children.insert({ship_ptr->get_name(), ship_ptr});
}

void Ship_composite::remove_component(const string& name) {
    children.erase(name);
}

shared_ptr<Ship_component> Ship_composite::get_child(const string& name) {
    auto child_itt = children.find(name);
    if (child_itt == children.end())
        return nullptr;
    else
        return child_itt->second.lock();
}

bool Ship_composite::can_move() const {
    throw Error("Group boolean!");
}

bool Ship_composite::is_moving() const {
    throw Error("Group boolean!");
}

bool Ship_composite::is_docked() const {
    throw Error("Group boolean!");
}

bool Ship_composite::is_afloat() const {
    throw Error("Group boolean!");
}

bool Ship_composite::can_dock(shared_ptr<Island>) const {
    throw Error("Group boolean!");
}

Point Ship_composite::get_location() const {
    throw Error("Can't get the location of a group!");
}

shared_ptr<Island> Ship_composite::get_docked_Island() const {
    throw Error("Can't get member of group!");
}

shared_ptr<Island> Ship_composite::get_destination_Island() const {
    throw Error("Can't get member of group!");
}

double Ship_composite::get_maximum_speed() const {
    throw Error("Can't get max speed of a group!");
}

void Ship_composite::update() { }

void Ship_composite::describe() const {
    cout << "Group " << get_name() << endl;
}

void Ship_composite::broadcast_current_state() const { }

void Ship_composite::set_destination_position_and_speed(
        Point destination_position, double speed) {
    for (auto& map_pair : children) {
        try {
            map_pair.second.lock()->set_destination_position_and_speed(
                    destination_position, speed);
        } catch (Error& e) {
            cout << e.what() << endl;
        }
    }
}

void Ship_composite::set_destination_island_and_speed(
        shared_ptr<Island> destination_island, double speed) {
    for (auto& map_pair : children) {
        try {
            map_pair.second.lock()->set_destination_island_and_speed(
                    destination_island, speed);
        } catch (Error& e) {
            cout << e.what() << endl;
        }
    }
}

void Ship_composite::set_course_and_speed(double course, double speed) {
    for (auto& map_pair : children) {
        try {
            map_pair.second.lock()->set_course_and_speed(course, speed);
        } catch (Error& e) {
            cout << e.what() << endl;
        }
    }
}

void Ship_composite::stop() {
    for (auto& map_pair : children) {
        map_pair.second.lock()->stop();
    }
}

void Ship_composite::dock(shared_ptr<Island> island_ptr) {
    for (auto& map_pair : children) {
        try {
            map_pair.second.lock()->dock(island_ptr);
        } catch (Error& e) {
            cout << e.what() << endl;
        }
    }
}

void Ship_composite::refuel() {
    for (auto& map_pair : children) {
        try {
            map_pair.second.lock()->refuel();
        } catch (Error& e) {
            cout << e.what() << endl;
        }
    }
}

void Ship_composite::receive_hit(int, shared_ptr<Ship>) {
    throw Error("A group can't take a hit!");
}

void Ship_composite::set_load_destination(shared_ptr<Island> island_ptr) {
    for (auto& map_pair : children) {
        try {
            map_pair.second.lock()->set_load_destination(island_ptr);
        } catch (Error& e) {
            cout << e.what() << endl;
        }
    }
}

void Ship_composite::set_unload_destination(shared_ptr<Island> island_ptr) {
    for (auto& map_pair : children) {
        try {
            map_pair.second.lock()->set_unload_destination(island_ptr);
        } catch (Error& e) {
            cout << e.what() << endl;
        }
    }
}

void Ship_composite::attack(shared_ptr<Ship> target_ptr) {
    for (auto& map_pair : children) {
        try {
            map_pair.second.lock()->attack(target_ptr);
        } catch (Error& e) {
            cout << e.what() << endl;
        }
    }
}

void Ship_composite::stop_attack() {
    for (auto& map_pair : children) {
        try {
            map_pair.second.lock()->stop_attack();
        } catch (Error& e) {
            cout << e.what() << endl;
        }
    }
}

void Ship_composite::start_skimming(Point spill_origin_, int spill_size_) {
    for (auto& map_pair : children) {
        try {
            map_pair.second.lock()->start_skimming(spill_origin_, spill_size_);
        } catch (Error& e) {
            cout << e.what() << endl;
        }
    }
}

bool Ship_composite::children_if_helper(
        const ChildrenMap_t::value_type& map_pair, bool (Ship_component::* func_ptr)()) const {
    return (map_pair.second.lock().get()->*func_ptr)();
}
