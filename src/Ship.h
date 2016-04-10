#ifndef SHIP_H
#define SHIP_H
/***** Ship Class *****/
/* A Ship has a name, initial position, amount of fuel, and parameters that govern its movement.
The initial amount of fuel is equal to the supplied fuel capacity - a full fuel tank.
A Ship can be commanded to move to either a position, and Island, or follow a course, or stop,
dock at or refuel at an Island. It consumes fuel while moving, and becomes immobile
if it runs out of fuel. It inherits the Sim_object interface to the rest of the system,
and the Track_base class provides the basic movement functionality, with the unit of time
corresponding to 1.0 for one "tick" - an hour of simulated time. The speeds and rates
are specified as per unit time, but in this project, the update time is always 1.0.

The update function updates the position and/or state of the ship.
The describe function outputs information about the ship state.
Accessors make the ship state available to either the public or to derived classes.
The is a "fat interface" for the capabilities of derived types of Ships. These
functions are implemented in this class to throw an Error exception.
*/

#include <memory>

#include "Track_base.h"
#include "Ship_component.h"

class Island;

class Ship : public Ship_component, public std::enable_shared_from_this<Ship> {

public:
    /*** Readers ***/
    // return the current position
    Point get_location() const override { return track_base.get_position(); }

    // Return true if ship can move (it is not dead in the water or in the process or sinking);
    bool can_move() const override;

    // Return true if ship is moving
    bool is_moving() const override;

    // Return true if ship is docked
    bool is_docked() const override;

    // Return true if ship is afloat (not in process of sinking), false if not
    bool is_afloat() const override;

    // Return true if the ship is Stopped and the distance to the supplied island
    // is less than or equal to 0.1 nm
    bool can_dock(std::shared_ptr<Island> island_ptr) const override;

    // Get the maximum speed for the ship
    double get_maximum_speed() const override
    { return maximum_speed; }

    // return pointer to the Island currently docked at, or nullptr if not docked
    std::shared_ptr<Island> get_docked_Island() const override
    { return docked_Island; }

    // return pointer to current destination Island, nullptr if not set
    std::shared_ptr<Island> get_destination_Island() const override
    { return destination_Island; }

    /*** Interface to derived classes ***/
    // Update the state of the Ship
    void update() override;

    // output a description of current state to cout
    void describe() const override;

    // Broadcast the state of the ship to the Model
    void broadcast_current_state() const override;

    /*** Command functions ***/
    virtual // Start moving to a destination position at a speed
    // may throw Error("Ship cannot move!")
    // may throw Error("Ship cannot go that fast!")
    void set_destination_position_and_speed(Point destination_position,
            double speed) override;

    virtual // Start moving to a destination Island at a speed
    // may throw Error("Ship cannot move!")
    // may throw Error("Ship cannot go that fast!")
    void set_destination_island_and_speed(std::shared_ptr<Island> destination_island,
            double speed) override;

    virtual // Start moving on a course and speed
    // may throw Error("Ship cannot move!")
    // may throw Error("Ship cannot go that fast!");
    void set_course_and_speed(double course, double speed) override;

    // Stop moving
    // may throw Error("Ship cannot move!");
    void stop() override;

    // dock at an Island - set our position = Island's position, go into Docked state
    // may throw Error("Can't dock!");
    void dock(std::shared_ptr<Island> island_ptr) override;

    // Refuel - must already be docked at an island; fill takes as much as possible
    // may throw Error("Must be docked!");
    void refuel() override;

    // interactions with other objects
    // receive a hit from an attacker
    void receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr) override;

    /*** Fat interface command functions ***/
    // These functions throw an Error exception for this class
    // will always throw Error("Cannot load at a destination!");
    void set_load_destination(std::shared_ptr<Island>) override;

    // will always throw Error("Cannot unload at a destination!");
    void set_unload_destination(std::shared_ptr<Island>) override;

    // will always throw Error("Cannot attack!");
    void attack(std::shared_ptr<Ship> in_target_ptr) override;

    // will always throw Error("Cannot attack!");
    void stop_attack() override;

    // will always throw Error("Cannot skim!");
    void start_skimming(Point spill_origin_, int spill_size_) override;

protected:
    // Protected to prevent construction of plain ship objects
    Ship(const std::string &name_, Point position_, double fuel_capacity_,
            double maximum_speed_, double fuel_consumption_, int resistance_);

private:
    double fuel;                        // Current amount of fuel
    double fuel_capacity;               // Amount of fuel ship can hold
    double fuel_consumption;            // tons/nm required per cycle
    Point destination_point = {0., 0.};  // Current destination position
    std::shared_ptr<Island> docked_Island;    // Island we're docked at
    std::shared_ptr<Island> destination_Island; // Current destination Island, if any
    enum class State_t {
        docked, stopped, moving_on_course, dead_in_the_water,
        moving_to_position, moving_to_island, sunk
    } ship_state = State_t::stopped;      // State of the ship
    double maximum_speed;               // Maximum speed the ship supports
    int resistance;                     // Resistance to damage for the ship
    Track_base track_base;              // Location tracking and navigation


    // Updates position, fuel, and movement_state, assuming 1 time unit (1 hr)
    void calculate_movement();

    // Reset the destination point, destination island, and docked island
    void reset_destinations_and_dock();

    // Set the speed after ensuring that we can move at the requested speed
    void set_speed_with_check(double speed);

    // Set the position of the ship and notify the Model about it.
    void set_position(Point point);

    // Set the course of the ship and notify the Model about it.
    void set_course(double course);

    // Set the speed of the ship and notify the Model about it.
    void set_speed(double speed);

    // Set the fuel level of the ship and notify the Model about it.
    void set_fuel(double fuel);
};

#endif