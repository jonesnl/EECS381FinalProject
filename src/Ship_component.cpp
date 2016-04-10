#include "Ship_component.h"

#include "Ship_composite.h"

#include "Utility.h"

using namespace std;

Ship_component::Ship_component(const string &name) :
        Sim_object {name} { }

Ship_component::~Ship_component() {
    auto parent_ptr = parent.lock();
    if (parent_ptr) {
        parent_ptr->remove_component(get_name());
    }
}

void Ship_component::add_component(shared_ptr<Ship_component>) {
    throw Error("Can't add a component!");
}

void Ship_component::remove_component(const string&) {
    throw Error("Can't remove a component!");
}

shared_ptr<Ship_component> Ship_component::get_child(const string&) {
    throw Error("Can't get a child!");
}

void Ship_component::add_parent(shared_ptr<Ship_composite> parent_) {
    if (parent.lock())
        throw Error("Component already has a parent!");

    parent = parent_;
}

void Ship_component::remove_parent() {
    parent.reset();
}