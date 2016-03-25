#include "Model.h"

#include "Ship.h"
#include "Island.h"
#include "Ship_factory.h"
#include "Utility.h"
#include "View.h"

#include <iostream>
#include <algorithm>

using namespace std;
using namespace std::placeholders;

Model *Model::singleton_ptr = nullptr;

Model_destroyer model_destroyer;

Model::Model() {
    insert_island(make_shared<Island>("Exxon", Point(10, 10), 1000, 200));
    insert_island(make_shared<Island>("Shell", Point(0, 30), 1000, 200));
    insert_island(make_shared<Island>("Bermuda", Point(20, 20)));
    insert_island(make_shared<Island>("Treasure_Island", Point(50, 5), 100, 5));

    insert_ship(create_ship("Ajax", "Cruiser", Point(15, 15)));
    insert_ship(create_ship("Xerxes", "Cruiser", Point(25, 25)));
    insert_ship(create_ship("Valdez", "Tanker", Point(30, 30)));

    cout << "Model constructed" << endl;
}

Model::~Model() {
    cout << "Model destructed" << endl;
}

bool Model::is_name_in_use(const std::string& name) const {
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

shared_ptr<Island> Model::get_island_ptr(const std::string& name) const {
    auto itt = island_map.find(name);
    if (itt == island_map.end())
        throw Error("Island not found!");
    return itt->second;
}

Model::IslandSet_t Model::get_set_of_islands() const {
    IslandSet_t island_set;
    transform(island_map.begin(), island_map.end(),
            inserter(island_set, island_set.begin()),
            mem_fn(&IslandMap_t::value_type::second));
    return island_set;
}

bool Model::is_ship_present(const std::string& name) const {
    return ship_map.find(name) != ship_map.end();
}

void Model::add_ship(std::shared_ptr<Ship> ship_ptr) {
    insert_ship(ship_ptr);
    ship_ptr->broadcast_current_state();
}

void Model::remove_ship(shared_ptr<Ship> ship_ptr) {
    const string& ship_name = ship_ptr->get_name();
    ship_map.erase(ship_name);
    object_map.erase(ship_name);

}

shared_ptr<Ship> Model::get_ship_ptr(const std::string& name) const {
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
    time += 1;
}

void Model::attach(shared_ptr<View> view) {
    view_set.insert(view);
    for_each(object_map.begin(), object_map.end(),
            bind(&Sim_object::broadcast_current_state,
                    bind(&ObjectMap_t::value_type::second, _1)));
}

void Model::detach(shared_ptr<View> view) {
    view_set.erase(view);
}

void Model::notify_location(const std::string& name, Point location) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_location, _1, name, location));
}

void Model::notify_course(const std::string& name, double course) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_course, _1, name, course));
}

void Model::notify_speed(const std::string& name, double speed) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_speed, _1, name, speed));
}

void Model::notify_fuel(const std::string& name, double fuel) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_fuel, _1, name, fuel));
}

void Model::notify_gone(const std::string& name) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_remove, _1, name));
}

Model *Model::get_Instance() {
    if (!singleton_ptr)
        singleton_ptr = new Model();
    return singleton_ptr;
}

/* Private member functions */

void Model::insert_island(shared_ptr<Island> island) {
    island_map.insert({island->get_name(), island});
    object_map.insert({island->get_name(), island});
}

void Model::insert_ship(shared_ptr<Ship> ship_ptr) {
    ship_map.insert({ship_ptr->get_name(), ship_ptr});
    object_map.insert({ship_ptr->get_name(), ship_ptr});
}

Model_destroyer::~Model_destroyer() {
    if (Model::singleton_ptr)
        delete Model::singleton_ptr;
}