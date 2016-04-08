#ifndef SHIP_COMPONENT_H
#define SHIP_COMPONENT_H

#include "Sim_object.h"
#include "Geometry.h"

#include <memory>

class Island;
class Ship;
class Ship_composite;

class Ship_component : public Sim_object {
public:
    Ship_component(const std::string& name);

    /*** Component functions ***/
    virtual void add_component();

    virtual void remove_component(const std::string& name);

    virtual std::shared_ptr<Ship_component> get_child(const std::string& name);

    /*** Readers ***/
    // Return true if ship can move (it is not dead in the water or in the process or sinking);
    virtual bool can_move() const = 0;

    // Return true if ship is moving
    virtual bool is_moving() const = 0;

    // Return true if ship is docked
    virtual bool is_docked() const = 0;

    // Return true if ship is afloat (not in process of sinking), false if not
    virtual bool is_afloat() const = 0;

    // Return true if the ship is Stopped and the distance to the supplied island
    // is less than or equal to 0.1 nm
    virtual bool can_dock(std::shared_ptr <Island> island_ptr) const = 0;

    /*** Command functions ***/
    // Start moving to a destination position at a speed
    // may throw Error("Ship cannot move!")
    // may throw Error("Ship cannot go that fast!")
    virtual void set_destination_position_and_speed(Point destination_position,
            double speed) = 0;

    // Start moving to a destination Island at a speed
    // may throw Error("Ship cannot move!")
    // may throw Error("Ship cannot go that fast!")
    virtual void set_destination_island_and_speed(
            std::shared_ptr <Island> destination_island, double speed) = 0;

    // Start moving on a course and speed
    // may throw Error("Ship cannot move!")
    // may throw Error("Ship cannot go that fast!");
    virtual void set_course_and_speed(double course, double speed) = 0;

    // Stop moving
    // may throw Error("Ship cannot move!");
    virtual void stop() = 0;

    // dock at an Island - set our position = Island's position, go into Docked state
    // may throw Error("Can't dock!");
    virtual void dock(std::shared_ptr <Island> island_ptr) = 0;

    // Refuel - must already be docked at an island; fill takes as much as possible
    // may throw Error("Must be docked!");
    virtual void refuel() = 0;

    /*** Fat interface command functions ***/
    // These functions throw an Error exception for this class
    // will always throw Error("Cannot load at a destination!");
    virtual void set_load_destination(std::shared_ptr <Island> island_ptr) = 0;

    // will always throw Error("Cannot unload at a destination!");
    virtual void set_unload_destination(std::shared_ptr <Island> island_ptr) = 0;

    // will always throw Error("Cannot attack!");
    virtual void attack(std::shared_ptr<Ship> target_ptr) = 0;

    // will always throw Error("Cannot attack!");
    virtual void stop_attack() = 0;

    // will always throw Error("Cannot skim!");
    virtual void start_skimming(Point spill_origin_, int spill_size_) = 0;

    // interactions with other objects
    // receive a hit from an attacker
    virtual void receive_hit(int hit_force, std::shared_ptr <Ship> attacker_ptr) = 0;
protected:
    // Get the maximum speed for the ship
    virtual double get_maximum_speed() const = 0;

    // return pointer to the Island currently docked at, or nullptr if not docked
    virtual std::shared_ptr<Island> get_docked_Island() const = 0;

    // return pointer to current destination Island, nullptr if not set
    virtual std::shared_ptr<Island> get_destination_Island() const = 0;

private:
    std::weak_ptr<Ship_composite> parent; // TODO
};

#endif
