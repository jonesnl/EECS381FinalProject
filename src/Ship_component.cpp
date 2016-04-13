#include "Ship_component.h"

#include "Ship_group.h"

#include "Utility.h"

using namespace std;

Ship_component::Ship_component(const string &name) :
        Sim_object {name} { }

Ship_component::~Ship_component() {
    auto parent_ptr = parent.lock();
    if (parent_ptr) {
        parent_ptr->remove_child(shared_from_this());
    }
}

void Ship_component::add_child(shared_ptr<Ship_component>) {
    throw Error("Can't add a child!");
}

void Ship_component::remove_child(shared_ptr<Ship_component>) {
    throw Error("Can't remove a child!");
}

shared_ptr<Ship_component> Ship_component::get_child(const string&) {
    throw Error("Can't get a child!");
}

void Ship_component::add_parent(shared_ptr<Ship_group> parent_) {
    if (parent.lock())
        throw Error("Component already has a parent!");
    parent = parent_;
}

shared_ptr<Ship_component> Ship_component::get_parent() {
    return parent.lock();
}

void Ship_component::remove_parent() {
    parent.reset();
}

