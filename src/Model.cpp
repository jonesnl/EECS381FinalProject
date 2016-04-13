#include "Model.h"

#include "Ship_component.h"
#include "Island.h"
#include "Ship_factory.h"
#include "Utility.h"
#include "View.h"

#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;
using namespace std::placeholders;

/*
 * Model contains the data for the simulation. The singleton is created by the
 * Model::get_inst() function and is destroyed by the Model_destroyer destructor
 * in the static global Model_destroyer object at the end of the program's life.
 */

/*************** Model ***************/
// Initial value of Model's singleton pointer.
Model *Model::singleton_ptr = nullptr;

/* Public member functions*/

// Construct the basis of our simulation.
Model::Model() {
    insert_island(make_shared<Island>("Exxon", Point(10, 10), 1000, 200));
    insert_island(make_shared<Island>("Shell", Point(0, 30), 1000, 200));
    insert_island(make_shared<Island>("Bermuda", Point(20, 20)));
    insert_island(make_shared<Island>("Treasure_Island", Point(50, 5), 100, 5));

    insert_ship(create_ship("Ajax", "Cruiser", Point(15, 15)));
    insert_ship(create_ship("Xerxes", "Cruiser", Point(25, 25)));
    insert_ship(create_ship("Valdez", "Tanker", Point(30, 30)));
}

// Check if a name is in use
bool Model::is_name_in_use(const std::string& name) const {
    // lower_bound will match an abbreviated name with an object with the same
    // abbreviation if such an object exists in the object_map. If the abbreviation of the object
    // is the same as the found object in the object map's name, then the name is in use.
    string abrv_name = name.substr(0, name_abbreviation_length_c);
    auto itt = object_map.lower_bound(abrv_name);
    return itt != object_map.end() &&
            itt->first.substr(0, name_abbreviation_length_c) == abrv_name;
}

// Checks if an island exists in the simulation
bool Model::is_island_present(const std::string& name) const {
    return island_map.find(name) != island_map.end();
}

// Gets an island_ptr from the island_map based on the island's name.
shared_ptr<Island> Model::get_island_ptr(const std::string& name) const {
    auto itt = island_map.find(name);
    if (itt == island_map.end())
        throw Error("Island not found!");
    return itt->second;
}

// Gets a vector of all Island pointers from the model.
vector<shared_ptr<Island>> Model::get_vector_of_islands() const {
    vector<shared_ptr<Island>> island_vect;
    transform(island_map.begin(), island_map.end(),
            back_inserter(island_vect),
            mem_fn(&IslandMap_t::value_type::second));
    return island_vect;
}

// Checks if a ship currently exists in the simulation
bool Model::is_ship_present(const std::string& name) const {
    return ship_map.find(name) != ship_map.end();
}

// Adds a ship to the model, and updates all the views
void Model::add_ship(std::shared_ptr<Ship_component> ship_ptr) {
    insert_ship(ship_ptr);
    ship_ptr->broadcast_current_state();
}

// Get a ship's pointer based on the name of the ship.
shared_ptr<Ship_component> Model::get_ship_ptr(const string& name) const {
    auto itt = ship_map.find(name);
    if (itt == ship_map.end())
        throw Error("Ship not found!");
    return itt->second;
}

// Removes a ship from the model.
void Model::remove_ship(shared_ptr<Ship_component> ship_ptr) {
    ship_map.erase(ship_ptr->get_name());
    object_map.erase(ship_ptr->get_name());
}

// Describe all objects in the model in alphabetical order.
void Model::describe() const {
    for_each(object_map.begin(), object_map.end(),
            bind(&Sim_object::describe,
                    bind(&ObjectMap_t::value_type::second, _1)));
}

// Simulate the next time step by incrementing the time, and updating all objects
// in the model.
void Model::update() {
    time += 1;
    for_each(object_map.begin(), object_map.end(),
            bind(&Sim_object::update,
                    bind(&ObjectMap_t::value_type::second, _1)));
}

// Attach a view to the model, and tell all objects to broadcast their state
// so the view can be populated with data.
void Model::attach(shared_ptr<View> view) {
    view_set.insert(view);
    for_each(object_map.begin(), object_map.end(),
            bind(&Sim_object::broadcast_current_state,
                    bind(&ObjectMap_t::value_type::second, _1)));
}

// Detach a view from the model.
void Model::detach(shared_ptr<View> view) {
    view_set.erase(view);
}

// Notify views of an object's location.
void Model::notify_location(const std::string& name, Point location) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_location, _1, name, location));
}

// Notify views of an object's course.
void Model::notify_course(const std::string& name, double course) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_course, _1, name, course));
}

// Notify views of an object's speed.
void Model::notify_speed(const std::string& name, double speed) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_speed, _1, name, speed));
}

// Notify views of an object's fuel.
void Model::notify_fuel(const std::string& name, double fuel) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_fuel, _1, name, fuel));
}

// Notify views that an object is no longer in the simulation
void Model::notify_gone(const std::string& name) {
    for_each(view_set.begin(), view_set.end(),
            bind(&View::update_remove, _1, name));
}

// Get the singleton pointer for the Model, create the singleton if it has not
// yet been created.
Model *Model::get_inst() {
    if (!singleton_ptr)
        singleton_ptr = new Model();
    return singleton_ptr;
}

/* Private member functions */
// Add an island to relevant data structures
void Model::insert_island(shared_ptr<Island> island) {
    island_map.insert({island->get_name(), island});
    object_map.insert({island->get_name(), island});
}

// Add a ship to relevant data structures
void Model::insert_ship(shared_ptr<Ship_component> ship_ptr) {
    assert(!is_name_in_use(ship_ptr->get_name()));
    ship_map.insert({ship_ptr->get_name(), ship_ptr});
    object_map.insert({ship_ptr->get_name(), ship_ptr});
}

/******* Model_destroyer ********/
// Static object that when destroyed will destroy Model's singleton object during
// program shutdown.
static Model_destroyer model_destroyer;

// Destroy Model's singleton
Model_destroyer::~Model_destroyer() {
    if (Model::singleton_ptr)
        delete Model::singleton_ptr;
}
