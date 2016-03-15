#include "Model.h"

#include "Ship.h"
#include "Island.h"
#include "Ship_factory.h"
#include "Utility.h"
#include "View.h"

#include <iostream>
#include <algorithm>
#include <memory>

using namespace std;
using namespace placeholders;

Model* g_Model_ptr;

Model::Model() {
    add_island(new Island("Exxon", Point(10, 10), 1000, 200));
    add_island(new Island("Shell", Point(0, 30), 1000, 200));
    add_island(new Island("Bermuda", Point(20, 20)));

    add_ship(create_ship("Ajax", "Cruiser", Point(15, 15)));
    add_ship(create_ship("Xerxes", "Cruiser", Point(25, 25)));
    add_ship(create_ship("Valdez", "Tanker", Point(30, 30)));

    cout << "Model constructed" << endl;
}

Model::~Model() {
    for_each(object_map.begin(), object_map.end(),
            [](ObjectMap_t::value_type& pair) {
                delete pair.second;
            });
    cout << "Model destructed" << endl;
}

bool Model::is_name_in_use(const std::string& name) const {
    if (name.length() < 2)
        return true;
    string abrv_name = name.substr(0, 2);
    auto itt = object_map.lower_bound(abrv_name);
    if (itt != object_map.end() &&
            itt->second->get_name().substr(0, 2) == abrv_name)
        return true;
    else
        return false;
}

bool Model::is_island_present(const std::string& name) const {
    return island_map.find(name) != island_map.end();
}

Island *Model::get_island_ptr(const std::string& name) const {
    auto itt = island_map.find(name);
    if (itt == island_map.end())
        throw Error("Island not found!");
    return itt->second;
}

bool Model::is_ship_present(const std::string& name) const {
    return ship_map.find(name) != ship_map.end();
}

void Model::add_ship(Ship *ship) {
    ship_map.insert({ship->get_name(), ship});
    object_map.insert({ship->get_name(), ship});
    notify_location(ship->get_name(), ship->get_location());
}

Ship *Model::get_ship_ptr(const std::string& name) const {
    auto itt = ship_map.find(name);
    if (itt == ship_map.end())
        throw Error("Ship not found!");
    return itt->second;
}

void Model::describe() const {
    for_each(object_map.begin(), object_map.end(),
            bind(&Sim_object::describe,
                    bind(&ObjectMap_t::value_type::second, _1)));
}

void Model::update() {
    for_each(object_map.begin(), object_map.end(),
            bind(&Sim_object::update,
                    bind(&ObjectMap_t::value_type::second, _1)));

    for (auto itt = ship_map.begin(); itt != ship_map.end(); ) {
        if (itt->second->is_on_the_bottom()) {
            Ship *ptr = itt->second;
            itt = ship_map.erase(itt);
            object_map.erase(ptr->get_name());
            delete ptr;
        } else {
            ++itt;
        }
    }
    time += 1;
}

void Model::attach(View *view) {
    view_set.insert(view);
    for_each(object_map.begin(), object_map.end(),
            bind(&Sim_object::broadcast_current_state,
                    bind(&ObjectMap_t::value_type::second, _1)));
}

void Model::detach(View *view) {
    view_set.erase(view);
}

void Model::notify_location(const std::string& name, Point location) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_location, _1, name, location));
}

void Model::notify_gone(const std::string& name) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_remove, _1, name));
    for (View *v : view_set) {
        v->update_remove(name);
    }
}

/* Private member functions */

void Model::add_island(Island *island) {
    island_map.insert({island->get_name(), island});
    object_map.insert({island->get_name(), island});
}
