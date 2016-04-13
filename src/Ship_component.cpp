#include "Ship_component.h"

#include "Ship_group.h"
#include "Utility.h"

using namespace std;

// Pass the object name up the hirarchy
Ship_component::Ship_component(const string &name) :
        Sim_object {name} { }

// Only groups support children
void Ship_component::add_child(shared_ptr<Ship_component>) {
    throw Error("Is not a group!");
}

// Only groups support children
void Ship_component::remove_child(shared_ptr<Ship_component>) {
    throw Error("Is not a group!");
}

// Only groups support children
void Ship_component::remove_all_children() {
    throw Error("Is not a group!");
}

// Only groups support children
shared_ptr<Ship_component> Ship_component::get_child(const string&) {
    throw Error("Is not a group!");
}

// Add a parent to the object. Makes sure that we don't already have a parent,
// and that the parent_ pointer is a group before storing.
void Ship_component::add_parent(shared_ptr<Ship_component> parent_) {
    if (get_parent())
        throw Error("Already has a parent!");
    auto parent_group_ptr = dynamic_pointer_cast<Ship_group>(parent_);
    if (!parent_group_ptr)
        throw Error("Parent is not a group!");
    parent = parent_group_ptr;
}

// Return the parent pointer
shared_ptr<Ship_component> Ship_component::get_parent() noexcept {
    return parent.lock();
}

// Remove the parent pointer
void Ship_component::remove_parent() noexcept {
    parent.reset();
}

