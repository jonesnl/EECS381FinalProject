#ifndef MODEL_H
#define MODEL_H

#include "Geometry.h"
#include "Navigation.h"
#include <string>
#include <map>
#include <set>
#include <memory>

/*
Model is part of a simplified Model-View-Controller pattern.
Model keeps track of the Sim_objects in our little world. It is the only
component that knows how many Islands and Ships there are, but it does not
know about any of their derived classes, nor which Ships are of what kind of Ship. 
It has facilities for looking up objects by name, and removing Ships.  When
created, it creates an initial group of Islands and Ships using the Ship_factory.
Finally, it keeps the system's time.

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

    using IslandSet_t = std::set<std::shared_ptr<Island>>;

    IslandSet_t get_set_of_islands() const;

    // is there such a ship?
    bool is_ship_present(const std::string &name) const;

    // add a new ship to the list, and update the view
    void add_ship(std::shared_ptr<Ship> ship);

    // remove the Ship from the containers.
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

    void notify_course_speed(const std::string& name, Course_speed cs);

    void notify_fuel(const std::string& name, double fuel);

    // notify the views that an object is now gone
    void notify_gone(const std::string &name);

    // get model pointer
    static Model *get_Instance();

    // disallow copy/move construction or assignment
    Model(const Model&) = delete;
    Model(const Model&&) = delete;
    Model& operator= (const Model&) = delete;
    Model& operator= (const Model&&) = delete;

    friend class Model_destroyer;
private:
    Model();
    ~Model();
    static Model *singleton_ptr;

    int time = 0;        // the simulated time
    using ObjectMap_t = std::map<std::string, std::shared_ptr<Sim_object>>;
    using ShipMap_t = std::map<std::string, std::shared_ptr<Ship>>;
    using IslandMap_t = std::map<std::string, std::shared_ptr<Island>>;
    using ViewSet_t = std::set<std::shared_ptr<View>>;
    ObjectMap_t object_map; // List of all objects
    ShipMap_t ship_map; // List of ships
    IslandMap_t island_map; // List of islands
    ViewSet_t view_set; // List of views

    void insert_island(std::shared_ptr<Island> island);
    void insert_ship(std::shared_ptr<Ship> ship);
};

class Model_destroyer {
public:
    ~Model_destroyer();
};

#endif
