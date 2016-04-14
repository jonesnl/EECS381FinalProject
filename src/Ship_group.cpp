#include <algorithm>
#include <iostream>
#include "Ship_group.h"
#include "Utility.h"

using namespace std;
using namespace placeholders;

/* Templated private member functions */

// Loop through all children, apply the provided function, and if an error
// occurs, print out the error message, but continue the loop.
template <typename T>
void Ship_group::for_each_child_catch(T func) const {
    for (auto& map_pair : children) {
        try {
            func(map_pair.second.lock());
        } catch (Error& e) {
            std::cout << map_pair.first << ": " << e.what() << std::endl;
        }
    }
}

/* Public member functions */

// Pass the group's name to the parent classes to store
Ship_group::Ship_group(const string& name_) :
        Ship_component(name_) { }

// Add a child to the children map, and set its parent pointer
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

// Remove a child from the children map, and clear its parent pointer
void Ship_group::remove_child(shared_ptr<Ship_component> child_ptr) {
    auto itt = children.find(child_ptr->get_name());
    if (itt == children.end())
        throw Error("No child with that name!");
    itt->second.lock()->remove_parent();
    children.erase(itt);
}

// Clear the children map after removing references to this group from the children's
// parents
void Ship_group::remove_all_children() {
    for (auto& map_pair : children) {
        map_pair.second.lock()->remove_parent();
    }
    children.clear();
}

// Get a child pointer using the child's name.
shared_ptr<Ship_component> Ship_group::get_child(const string& name) {
    auto child_itt = children.find(name);
    if (child_itt == children.end())
        return nullptr;
    else
        return child_itt->second.lock();
}

/* Ship Readers (all throw exceptions when called) */
bool Ship_group::can_move() const {
    throw Error("Is not an individual!");
}

bool Ship_group::is_moving() const {
    throw Error("Is not an individual!");
}

bool Ship_group::is_docked() const {
    throw Error("Is not an individual!");
}

bool Ship_group::is_afloat() const {
    throw Error("Is not an individual!");
}

bool Ship_group::can_dock(shared_ptr<Island>) const {
    throw Error("Is not an individual!");
}

Point Ship_group::get_location() const {
    throw Error("Is not an individual!");
}

shared_ptr<Island> Ship_group::get_docked_Island() const {
    throw Error("Is not an individual!");
}

shared_ptr<Island> Ship_group::get_destination_Island() const {
    throw Error("Is not an individual!");
}

double Ship_group::get_maximum_speed() const {
    throw Error("Is not an individual!");
}

/* Simulation and Model functions */

// Don't need to do anything during an update for a group
void Ship_group::update() { }

// Describe the ship using its name and a list of its immediate members
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

// Don't need to broadcast the state of the group
void Ship_group::broadcast_current_state() const { }

/* Command functions */
// All command functions call the corresponding command function on all children
// using the for_each_child_catch() helper function.

void Ship_group::set_destination_position_and_speed(
        Point destination_position, double speed) {
    for_each_child_catch(
            bind(&Ship_component::set_destination_position_and_speed, _1,
                    destination_position, speed));
}

void Ship_group::set_destination_island_and_speed(
        shared_ptr<Island> destination_island, double speed) {
    for_each_child_catch(
            bind(&Ship_component::set_destination_island_and_speed, _1,
                    destination_island, speed));
}

void Ship_group::set_course_and_speed(double course, double speed) {
    for_each_child_catch(
            bind(&Ship_component::set_course_and_speed, _1, course, speed));
}

void Ship_group::stop() {
    for_each_child_catch(mem_fn(&Ship_component::stop));
}

void Ship_group::dock(shared_ptr<Island> island_ptr) {
    for_each_child_catch(bind(&Ship_component::dock, _1, island_ptr));
}

void Ship_group::refuel() {
    for_each_child_catch(mem_fn(&Ship_component::refuel));
}

// Is not implemented since a group of unrelated ships should not be able to
// receive a hit from a single source.
void Ship_group::receive_hit(int, shared_ptr<Ship_component>) {
    throw Error("A group can't take a hit!");
}

void Ship_group::set_load_destination(shared_ptr<Island> island_ptr) {
    for_each_child_catch(
            bind(&Ship_component::set_load_destination, _1, island_ptr));
}

void Ship_group::set_unload_destination(shared_ptr<Island> island_ptr) {
    for_each_child_catch(
            bind(&Ship_component::set_unload_destination, _1, island_ptr));
}

void Ship_group::attack(shared_ptr<Ship_component> target_ptr) {
    for_each_child_catch(bind(&Ship_component::attack, _1, target_ptr));
}

void Ship_group::stop_attack() {
    for_each_child_catch(mem_fn(&Ship_component::stop_attack));
}

void Ship_group::start_skimming(Point spill_origin_, int spill_size_) {
    for_each_child_catch(bind(&Ship_component::start_skimming, _1,
            spill_origin_, spill_size_));
}

/* Private member functions */

// If a child is a member of either this group or a subgroup, return true, else return false
bool Ship_group::is_child_member(const string& name) const {
    for (const auto& map_pair : children) {
        auto component_ptr = map_pair.second.lock();
        // If the name matches, return true
        if (component_ptr->get_name() == name)
            return true;

        auto group_ptr = dynamic_pointer_cast<Ship_group>(component_ptr);
        // Recurse into child groups
        if (group_ptr && group_ptr->is_child_member(name))
            return true;
    }
    return false;
}
