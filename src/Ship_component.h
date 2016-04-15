#ifndef SHIP_COMPONENT_H
#define SHIP_COMPONENT_H

#include "Sim_object.h"
#include "Geometry.h"

#include <memory>

class Island;
class Ship_group;

/* Ship_component class
 *
 * The Ship_component class provides a unified interface for clients to use to
 * reference both Ships and ship groups.
 *
 * Group related functions have basic definitions in this class so that they do
 * not clutter derived Ship classes. Parent management is implimented in this class
 * since both Ships and ship groups can be children of a group.
 */

class Ship_component :
        public Sim_object, public std::enable_shared_from_this<Ship_component> {
public:
    // Construct a Ship_component, passing the name to the base Sim_object
    Ship_component(const std::string& name);

    /*** Group related functions ***/
    // Throws an Error if called, should be overrode by group class
    virtual void add_child(std::shared_ptr<Ship_component> ship_ptr);

    // Throws an Error if called, should be overrode by group class
    virtual void remove_child(std::shared_ptr<Ship_component> child_ptr);

    // Throws an Error if called, should be overrode by group class
    virtual void remove_all_children();

    // Throws an Error if called, should be overrode by group class
    virtual std::shared_ptr<Ship_component> get_child(const std::string& name);

    // Add a parent group to this object. If the object already has a parent or
    // parent_ is not the right type it throws an Error.
    virtual void add_parent(std::shared_ptr<Ship_component> parent_);

    // Return the parent pointer. Returns a null shared_ptr if the object does
    // not have a parent at the moment.
    virtual std::shared_ptr<Ship_component> get_parent() noexcept;

    // Remove the parent pointer if set. If not set, do nothing.
    virtual void remove_parent() noexcept;

    /* Fat interface for Ship and Ship_component */
    virtual void set_destination_position_and_speed(Point destination_position,
            double speed) = 0;
    virtual void set_destination_island_and_speed(
            std::shared_ptr <Island> destination_island, double speed) = 0;
    virtual void set_course_and_speed(double course, double speed) = 0;
    virtual void stop() = 0;
    virtual void dock(std::shared_ptr <Island> island_ptr) = 0;
    virtual void refuel() = 0;
    virtual void set_load_destination(std::shared_ptr <Island> island_ptr) = 0;
    virtual void set_unload_destination(std::shared_ptr <Island> island_ptr) = 0;
    virtual void attack(std::shared_ptr<Ship_component> target_ptr) = 0;
    virtual void stop_attack() = 0;
    virtual void start_skimming(Point spill_origin_, int spill_size_) = 0;
    virtual void receive_hit(int hit_force,
            std::shared_ptr<Ship_component> attacker_ptr) = 0;
private:
    // Pointer to the parent group of this object.
    std::weak_ptr<Ship_group> parent;
};

#endif
