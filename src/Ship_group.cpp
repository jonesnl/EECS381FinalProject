#include <algorithm>
#include <iostream>
#include "Ship_group.h"
#include "Utility.h"

using namespace std;
using namespace placeholders;

template <typename T>
void Ship_group::children_if_helper(T func) const {
    for (auto& map_pair : children) {
        try {
            func(map_pair.second.lock());
        } catch (Error& e) {
            std::cout << e.what() << std::endl;
        }
    }
}

// Pass the group's name to the parent classes to store
Ship_group::Ship_group(const string& name_) :
        Ship_component(name_) { }

// A
void Ship_group::add_child(shared_ptr<Ship_component> ship_ptr) {
    // If child-to-be is a group, make sure that we are not a member of that group,
    // else we create a cycle.
    auto composite_ptr = dynamic_pointer_cast<Ship_group>(ship_ptr);
    if (composite_ptr) {
        if (composite_ptr->is_child_member(get_name()))
            throw Error("Cycle detected!");
    }
    auto this_group_ptr = static_pointer_cast<Ship_group>(shared_from_this());
    // Will throw an error if the child already has a parent
    ship_ptr->add_parent(this_group_ptr);
    children.insert({ship_ptr->get_name(), ship_ptr});
}

void Ship_group::remove_child(shared_ptr<Ship_component> child_ptr) {
    auto itt = children.find(child_ptr->get_name());
    if (itt == children.end())
        throw Error("No child with that name!");
    itt->second.lock()->remove_parent();
    children.erase(itt);
}

shared_ptr<Ship_component> Ship_group::get_child(const string& name) {
    auto child_itt = children.find(name);
    if (child_itt == children.end())
        return nullptr;
    else
        return child_itt->second.lock();
}

bool Ship_group::can_move() const {
    throw Error("Group boolean!");
}

bool Ship_group::is_moving() const {
    throw Error("Group boolean!");
}

bool Ship_group::is_docked() const {
    throw Error("Group boolean!");
}

bool Ship_group::is_afloat() const {
    throw Error("Group boolean!");
}

bool Ship_group::can_dock(shared_ptr<Island>) const {
    throw Error("Group boolean!");
}

Point Ship_group::get_location() const {
    throw Error("Can't get the location of a group!");
}

shared_ptr<Island> Ship_group::get_docked_Island() const {
    throw Error("Can't get member of group!");
}

shared_ptr<Island> Ship_group::get_destination_Island() const {
    throw Error("Can't get member of group!");
}

double Ship_group::get_maximum_speed() const {
    throw Error("Can't get max speed of a group!");
}

void Ship_group::update() { }

void Ship_group::describe() const {
    cout << "\nGroup " << get_name() << endl;
    if (children.empty())
        cout << "No members" << endl;
    else {
        cout << "Group members: ";
        bool first_time = true;
        for (auto& s : children) {
            if (first_time) {
                cout << s.first;
                first_time = false;
            } else {
                cout << ", " << s.first;
            }
        }
        cout << endl;
    }
}

void Ship_group::broadcast_current_state() const { }

void Ship_group::set_destination_position_and_speed(
        Point destination_position, double speed) {
    children_if_helper(
            bind(&Ship_component::set_destination_position_and_speed, _1,
                    destination_position, speed));
}

void Ship_group::set_destination_island_and_speed(
        shared_ptr<Island> destination_island, double speed) {
    children_if_helper(
            bind(&Ship_component::set_destination_island_and_speed, _1,
                    destination_island, speed));
}

void Ship_group::set_course_and_speed(double course, double speed) {
    children_if_helper(
            bind(&Ship_component::set_course_and_speed, _1, course, speed));
}

void Ship_group::stop() {
    children_if_helper(mem_fn(&Ship_component::stop));
}

void Ship_group::dock(shared_ptr<Island> island_ptr) {
    children_if_helper(bind(&Ship_component::dock, _1, island_ptr));
}

void Ship_group::refuel() {
    children_if_helper(mem_fn(&Ship_component::refuel));
}

void Ship_group::receive_hit(int, shared_ptr<Ship_component>) {
    throw Error("A group can't take a hit!");
}

void Ship_group::set_load_destination(shared_ptr<Island> island_ptr) {
    children_if_helper(bind(&Ship_component::set_load_destination, _1, island_ptr));
}

void Ship_group::set_unload_destination(shared_ptr<Island> island_ptr) {
    children_if_helper(bind(&Ship_component::set_unload_destination, _1, island_ptr));
}

void Ship_group::attack(shared_ptr<Ship_component> target_ptr) {
    children_if_helper(bind(&Ship_component::attack, _1, target_ptr));
}

void Ship_group::stop_attack() {
    children_if_helper(mem_fn(&Ship_component::stop_attack));
}

void Ship_group::start_skimming(Point spill_origin_, int spill_size_) {
    children_if_helper(bind(&Ship_component::start_skimming, _1,
            spill_origin_, spill_size_));
}

bool Ship_group::is_child_member(const string& name) const {
    for (const auto& map_pair : children) {
        auto composite_ptr = dynamic_pointer_cast<Ship_group>(map_pair.second.lock());
        if (composite_ptr) {
            if (composite_ptr->get_name() == name)
                return true;
            else if (composite_ptr->is_child_member(name))
                return true;
        }
    }
    return false;
}
