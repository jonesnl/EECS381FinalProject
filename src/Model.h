#ifndef MODEL_H
#define MODEL_H

#include "Geometry.h"
#include "Navigation.h"

#include <string>
#include <set>
#include <map>
#include <memory>
#include <vector>

/*
Model is part of a simplified Model-View-Controller pattern.
Model keeps track of the Sim_objects in our little world. It is the only
component that knows how many Islands and Ships there are, but it does not
know about any of their derived classes, nor which Ships are of what kind of Ship. 
It has facilities for looking up objects by name, fetching a vector of islands in
the simulation, and removing Ships.  When created, it creates an initial group
of Islands and Ships using the Ship_factory. Finally, it keeps the system's time.

Controller tells Model what to do; Model in turn tells the objects what do, and
when asked to do so by an object, tells all the Views whenever anything changes that might be relevant.
Model also provides facilities for looking up objects given their name.
*/

class Model;
class Ship;
class View;
class Island;
class Sim_object;

class Model {
public:
    // return the current time
    int get_time() { return time; }

    // is name already in use for either ship or island?
    // either the identical name, or identical in first two characters counts as in-use
    bool is_name_in_use(const std::string &name) const;

    // is there such an island?
    bool is_island_present(const std::string &name) const;

    // will throw Error("Island not found!") if no island of that name
    std::shared_ptr<Island> get_island_ptr(const std::string &name) const;

    // Get vector of island pointers, sorted by island name
    std::vector<std::shared_ptr<Island>> get_vector_of_islands() const;

    // Get vector of Tanker pointers, sorted by ship name
    std::vector<std::shared_ptr<Ship>> get_vector_of_ships() const;

    // is there such a ship?
    bool is_ship_present(const std::string &name) const;

    // add a new ship to the model, and updates the views
    void add_ship(std::shared_ptr<Ship> ship);

    // remove a Ship from the model.
    void remove_ship(std::shared_ptr<Ship> ship_ptr);

    // will throw Error("Ship not found!") if no ship of that name
    std::shared_ptr<Ship> get_ship_ptr(const std::string &name) const;

    // tell all objects to describe themselves
    void describe() const;

    // increment the time, and tell all objects to update themselves
    void update();

    /* View services */
    // Attaching a View adds it to the container and causes it to be updated
    // with all current objects'location (or other state information.
    void attach(std::shared_ptr<View> view);

    // Detach the View by discarding the supplied pointer from the container of Views
    // - no updates sent to it thereafter.
    void detach(std::shared_ptr<View> view);

    // notify the views about an object's location
    void notify_location(const std::string &name, Point location);

    // notify the views about a ship's course
    void notify_course(const std::string& name, double course);

    // notify the views about a ship's speed
    void notify_speed(const std::string& name, double speed);

    // notify the views about a ship's fuel
    void notify_fuel(const std::string& name, double fuel);

    // notify the views that an object is now gone
    void notify_gone(const std::string &name);

    // get model pointer singleton instance TODO remove local pointers
    static Model *get_inst();

    // disallow copy/move construction or assignment
    Model(const Model&) = delete;
    Model(const Model&&) = delete;
    Model& operator= (const Model&) = delete;
    Model& operator= (const Model&&) = delete;

    // Friend Model_destoryer to destroy the singleton_ptr
    friend class Model_destroyer;
private:
    // Private constructor to adhere to the singleton idiom.
    Model();

    // Pointer to the singleton Model object.
    static Model *singleton_ptr;

    int time = 0;        // the simulated time

    // Alias the data containers to some more meaningful names.
    using ObjectMap_t = std::map<std::string, std::shared_ptr<Sim_object>>;
    using ShipMap_t = std::map<std::string, std::shared_ptr<Ship>>;
    using IslandMap_t = std::map<std::string, std::shared_ptr<Island>>;
    using ViewSet_t = std::set<std::shared_ptr<View>>;

    ObjectMap_t object_map; // Map of all object pointers, key is the object's name
    ShipMap_t ship_map; // Map of ship pointers , key is the ship's name
    IslandMap_t island_map; // Map of island pointers, key is the island's name
    ViewSet_t view_set; // Set of view pointers

    // Insert an island to relevant containers
    void insert_island(std::shared_ptr<Island> island);

    // Insert a ship to relevant containers
    void insert_ship(std::shared_ptr<Ship> ship);
};

// Class used to destroy the Model's singleton object.
class Model_destroyer {
public:
    // Destroys Model's singleton object
    ~Model_destroyer();
};

#endif
