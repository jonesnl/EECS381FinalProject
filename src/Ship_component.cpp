#include "Ship_component.h"

#include "Utility.h"

using namespace std;

Ship_component::Ship_component(const string &name) :
        Sim_object {name} { }

void Ship_component::add_component(shared_ptr<Ship_component>) {
    throw Error("Can't add a component!");
}

void Ship_component::remove_component(const string&) {
    throw Error("Can't remove a component!");
}

shared_ptr<Ship_component> Ship_component::get_child(const string&) {
    throw Error("Can't get a child!");
}
