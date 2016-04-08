#include "Ship.h"

#include "Island.h"
#include "Model.h"
#include "Utility.h"

#include <iostream>
#include <cassert>

using namespace std;

/* Implementation of the Ship class. Handles navigation and resistance calculations
 * for derived ships.
 */

// If a ship is within the dock distance of an island, we can dock at that island
const double ship_dock_distance_c = 0.1;

/* Public Function Definitions */
// Return true if we can move
bool Ship::can_move() const {
    return is_afloat() && ship_state != State_t::dead_in_the_water;
}

// Return true if the ship is in a moving state
bool Ship::is_moving() const {
    return ship_state == State_t::moving_to_position ||
           ship_state == State_t::moving_to_island ||
           ship_state == State_t::moving_on_course;
}

// Return true if the ship is docked
bool Ship::is_docked() const {
    return ship_state == State_t::docked;
}

// Return true as long as the ship is not sunk
bool Ship::is_afloat() const {
    return ship_state != State_t::sunk;
}

// Return true if we are within range to dock at the provided island
bool Ship::can_dock(std::shared_ptr<Island> island_ptr) const {
    assert(island_ptr);
    double dist_to_island =
            cartesian_distance(island_ptr->get_location(), get_location());
    return ship_state == State_t::stopped && dist_to_island <= ship_dock_distance_c;
}

// Update the state of the ship, simulating a single time step
void Ship::update() {
    if (ship_state == State_t::sunk) {
        cout << get_name() << " sunk" << endl;
    } else if (is_moving()) {
        calculate_movement();
        cout << get_name() << " now at " << get_location() << endl;
        Model::get_inst()->notify_location(get_name(), get_location());
    } else if (ship_state == State_t::stopped) {
        cout << get_name() << " stopped at " << get_location() << endl;
    } else if (is_docked()) {
        cout << get_name() << " docked at " <<
                get_docked_Island()->get_name() << endl;
    } else if (ship_state == State_t::dead_in_the_water) {
        cout << get_name() << " dead in the water at " << get_location() << endl;
    }
}

// Describe the state of the ship
void Ship::describe() const {
    cout << get_name() << " at " << get_location();
    if (ship_state == State_t::sunk)
        cout << " sunk" << endl;
    else {
        cout << ", fuel: " << fuel << " tons, resistance: " << resistance << endl;
        if (ship_state == State_t::moving_to_position)
            cout << "Moving to " << destination_point <<
                    " on " << track_base.get_course_speed() << endl;
        else if (ship_state == State_t::moving_to_island)
            cout << "Moving to " << get_destination_Island()->get_name() <<
                    " on " << track_base.get_course_speed() << endl;
        else if (ship_state == State_t::moving_on_course)
            cout << "Moving on " << track_base.get_course_speed() << endl;
        else if (ship_state == State_t::docked)
            cout << "Docked at " << get_docked_Island()->get_name() << endl;
        else if (ship_state == State_t::stopped)
            cout << "Stopped" << endl;
        else if (ship_state == State_t::dead_in_the_water)
            cout << "Dead in the water" << endl;
    }
}

// Broadcast the state of the ship to the Model
void Ship::broadcast_current_state() const {
    Model::get_inst()->notify_location(get_name(), get_location());
    Model::get_inst()->notify_course(get_name(), track_base.get_course());
    Model::get_inst()->notify_speed(get_name(), track_base.get_speed());
    Model::get_inst()->notify_fuel(get_name(), fuel);
}

/*** Command functions ***/

// Tell the ship to travel to a position at a speed
void Ship::set_destination_position_and_speed(Point destination_position,
        double speed) {
    // Set the speed and course
    set_speed_with_check(speed);
    Compass_vector vect {get_location(), destination_position};
    set_course(vect.direction);
    // Reset any old destinations that may have been set
    reset_destinations_and_dock();
    // Set our new destination
    ship_state = State_t::moving_to_position;
    destination_point = destination_position;
    cout << get_name() << " will sail on " << track_base.get_course_speed() <<
            " to " << destination_position << endl;
}

// Tell the ship to travel to an island at a speed
void Ship::set_destination_island_and_speed(shared_ptr<Island> destination_island,
        double speed) {
    // Set the speed and the course
    set_speed_with_check(speed);
    Compass_vector vect {get_location(), destination_island->get_location()};
    set_course(vect.direction);
    // Reset any old destinations that may have been set
    reset_destinations_and_dock();
    // Set our new destination
    ship_state = State_t::moving_to_island;
    destination_Island = destination_island;
    destination_point = destination_island->get_location();
    cout << get_name() << " will sail on " << track_base.get_course_speed() <<
            " to " << destination_island->get_name() << endl;
}

// Tell the ship to travel at the provided course and speed
void Ship::set_course_and_speed(double course, double speed) {
    // Set the speed and the course
    set_speed_with_check(speed);
    set_course(course);
    // Reset any old destinations that may have been set
    reset_destinations_and_dock();
    // Set our new state
    ship_state = State_t::moving_on_course;
    cout << get_name() << " will sail on " << track_base.get_course_speed() << endl;
}

// Stop the ship
void Ship::stop() {
    set_speed_with_check(0.);
    reset_destinations_and_dock();
    ship_state = State_t::stopped;
    cout << get_name() << " stopping at " << get_location() << endl;
}

// Dock the ship at the provided island
void Ship::dock(shared_ptr<Island> island_ptr) {
    assert(island_ptr);
    if (!can_dock(island_ptr))
        throw Error("Can't dock!");
    set_position(island_ptr->get_location());
    docked_Island = island_ptr;
    ship_state = State_t::docked;
    set_speed(0.);
    cout << get_name() << " docked at " << island_ptr->get_name() << endl;
}

// Refuel at the docked island
void Ship::refuel() {
    if (!is_docked())
        throw Error("Must be docked!");
    double needed_fuel = fuel_capacity - fuel;
    if (needed_fuel < double_close_enough_c) {
        set_fuel(fuel_capacity);
        return;
    }
    set_fuel(fuel + get_docked_Island()->provide_fuel(needed_fuel));
    cout << get_name() << " now has " << fuel << " tons of fuel" << endl;
}

// Take a hit from a ship, which can sink us if our resistance goes below 0.
// We don't care about who attacked us, but functions that override this
// function might.
void Ship::receive_hit(int hit_force, shared_ptr<Ship>) {
    // We should never be getting hit if we are sunk
    assert(ship_state != State_t::sunk);
    // Take the hit
    resistance -= hit_force;
    cout << get_name() << " hit with " << hit_force << ", resistance now " <<
    resistance << endl;
    // If we have negative resistance, sink the ship
    if (is_afloat() && resistance < 0.) {
        Model *model_ptr = Model::get_inst();
        ship_state = State_t::sunk;
        set_speed(0.);
        cout << get_name() << " sunk" << endl;
        model_ptr->remove_ship(shared_from_this());
        model_ptr->notify_gone(get_name());
    }
}

// Throw error for fat interface functions
void Ship::set_load_destination(shared_ptr<Island>) {
    throw Error("Cannot load at a destination!");
}

// Throw error for fat interface functions
void Ship::set_unload_destination(shared_ptr<Island>) {
    throw Error("Cannot unload at a destination!");
}

// Throw error for fat interface functions
void Ship::attack(shared_ptr<Ship>) {
    throw Error("Cannot attack!");
}

// Throw error for fat interface functions
void Ship::stop_attack() {
    throw Error("Cannot attack!");
}

// Throw error for fat interface functions
void Ship::start_skimming(Point, int) {
    throw Error("Cannot skim!");
}

/* Protected Function Definitions */

// Construct a ship object using the parameters passed by the derived class.
Ship::Ship(const std::string &name_, Point position_, double fuel_capacity_,
        double maximum_speed_, double fuel_consumption_, int resistance_) :
        Ship_component(name_),
        fuel {fuel_capacity_}, fuel_capacity {fuel_capacity_},
        fuel_consumption {fuel_consumption_},
        maximum_speed {maximum_speed_},
        resistance {resistance_}, track_base(position_) { }


/* Private Function Definitions */

/*
Calculate the new position of a ship based on how it is moving, its speed, and
fuel state. This function should be called only if the state is 
moving_to_position, moving_to_island, or moving_on_course.

Track_base has an update_position(double time) function that computes the new position
of an object after the specified time has elapsed. If the Ship is going to move
for a full time unit (one hour), then it will get go the "full step" distance,
so update_position would be called with time = 1.0. If we can move less than that,
e.g. due to not enough fuel, update position  will be called with the corresponding
time less than 1.0.

For clarity in specifying the computation, this code assumes the specified private variable names, 
but you may change the variable names or enum class names, or state names if you wish (e.g. movement_state).
*/
void Ship::calculate_movement() {
    // Compute values for how much we need to move, and how much we can, and how long we can,
    // given the fuel state, then decide what to do.
    double time = 1.0;    // "full step" time
    // get the distance to destination
    double destination_distance = cartesian_distance(get_location(),
            destination_point);
    // get full step distance we can move on this time step
    double full_distance = track_base.get_speed() * time;
    // get fuel required for full step distance
    double full_fuel_required =
            full_distance * fuel_consumption;    // tons = nm * tons/nm
    // how far and how long can we sail in this time period based on the fuel state?
    double distance_possible, time_possible;
    if (full_fuel_required <= fuel) {
        distance_possible = full_distance;
        time_possible = time;
    }
    else {
        distance_possible = fuel / fuel_consumption;    // nm = tons / tons/nm
        time_possible = (distance_possible / full_distance) * time;
    }

    // are we are moving to a destination, and is the destination within the distance possible?
    if ((ship_state == State_t::moving_to_position ||
         ship_state == State_t::moving_to_island) &&
        destination_distance <= distance_possible) {
        // yes, make our new position the destination
        set_position(destination_point);
        // we travel the destination distance, using that much fuel
        double fuel_required = destination_distance * fuel_consumption;
        set_fuel(fuel - fuel_required);
        set_speed(0.);
        ship_state = State_t::stopped;
    }
    else {
        // go as far as we can, stay in the same movement state
        // simply move for the amount of time possible
        track_base.update_position(time_possible);
        // update_position does not call set_position, so send a notification
        // for the location that we are now at.
        Model::get_inst()->notify_location(get_name(), track_base.get_position());
        // have we used up our fuel?
        if (full_fuel_required >= fuel) {
            set_fuel(0.);
            set_speed(0.);
            ship_state = State_t::dead_in_the_water;
        }
        else {
            set_fuel(fuel - full_fuel_required);
        }
    }
}

// Reset the destination state, usually called if we change how the ship is
// moving in any of the set_destination functions, or by stopping the ship.
void Ship::reset_destinations_and_dock() {
    destination_point = {0., 0.};
    docked_Island = nullptr;
    destination_Island = nullptr;
}

// Check that we can travel at the provided speed, then set the speed
void Ship::set_speed_with_check(double speed) {
    if (speed > get_maximum_speed())
        throw Error("Ship cannot go that fast!");
    if (!can_move())
        throw Error("Ship cannot move!");
    set_speed(speed);
}

// Set the position of the ship and tell the model where we are
void Ship::set_position(Point point) {
    track_base.set_position(point);
    Model::get_inst()->notify_location(get_name(), point);
}

// Set the course and tell the model what it is
void Ship::set_course(double course) {
    track_base.set_course(course);
    Model::get_inst()->notify_course(get_name(), course);
}

// Set the speed adn tell the model what it is
void Ship::set_speed(double speed) {
    track_base.set_speed(speed);
    Model::get_inst()->notify_speed(get_name(), speed);
}

// Set the fuel and tell the model what it is
void Ship::set_fuel(double fuel_) {
    fuel = fuel_;
    Model::get_inst()->notify_fuel(get_name(), fuel);
}

