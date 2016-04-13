#include <cassert>
#include "Ship_component.h"

#include "Ship_group.h"

#include "Utility.h"

using namespace std;

Ship_component::Ship_component(const string &name) :
        Sim_object {name} { }

void Ship_component::add_child(shared_ptr<Ship_component>) {
    throw Error("Can't add a child!");
}

void Ship_component::remove_child(shared_ptr<Ship_component>) {
    throw Error("Can't remove a child!");
}

shared_ptr<Ship_component> Ship_component::get_child(const string&) {
    throw Error("Can't get a child!");
}

void Ship_component::add_parent(shared_ptr<Ship_component> parent_) {
    if (get_parent())
        throw Error("Component already has a parent!");
    auto parent_group_ptr = dynamic_pointer_cast<Ship_group>(parent_);
    if (!parent_group_ptr)
        throw Error("Parent pointer would not be a group!");
    parent = parent_group_ptr;
}

shared_ptr<Ship_component> Ship_component::get_parent() {
    return parent.lock();
}

void Ship_component::remove_parent() {
    parent.reset();
}

