#include <algorithm>
#include <iostream>
#include "Ship_composite.h"
#include "Utility.h"

using namespace std;
using namespace placeholders;

Ship_composite::Ship_composite(const string& name_) :
        Ship_component(name_) { }

void Ship_composite::add_component() {
    // TODO
}

void Ship_composite::remove_component(const string& name) {
    // TODO
}

shared_ptr<Ship_component> Ship_composite::get_child(const string& name) {
    // TODO
}

bool Ship_composite::can_move() const {
    return all_of(children.begin(), children.end(),
            bind(children_if_helper, _1, &Ship_component::can_move));
}

bool Ship_composite::is_moving() const {
    return any_of(children.begin(), children.end(),
            bind(children_if_helper, _1, &Ship_component::is_moving));
}

bool Ship_composite::is_docked() const {
    return all_of(children.begin(), children.end(),
            bind(children_if_helper, _1, &Ship_component::is_docked));
}

bool Ship_composite::is_afloat() const {
    return all_of(children.begin(), children.end(),
            bind(children_if_helper, _1, &Ship_component::is_afloat));
}

bool Ship_composite::can_dock(shared_ptr<Island> island_ptr) const {
    return all_of(children.begin(), children.end(),
            [island_ptr](const ChildrenMap_t::value_type& map_pair) {
                return map_pair.second.lock()->can_dock(island_ptr);
            });
}

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
