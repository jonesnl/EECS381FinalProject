#include "Ship.h"

#include "Island.h"
#include "Model.h"
#include "Utility.h"

#include <iostream>
#include <cassert>

using namespace std;

Ship::Ship(const std::string &name_, Point position_, double fuel_capacity_,
        double maximum_speed_, double fuel_consumption_, int resistance_) :
        Sim_object(name_),
        fuel {fuel_capacity_}, fuel_capacity {fuel_capacity_},
        fuel_consumption {fuel_consumption_},
        maximum_speed {maximum_speed_},
        resistance {resistance_}, track_base(position_) {
    cout << "Ship " << get_name() << " constructed" << endl;
}

/*
Define the destructor function even if it was declared as a pure virtual function.
This seems odd, because pure virtual functions are usually not defined in the class
that declares them. But this is often done as a way to make a class abstract,
if there is no other virtual function that makes sense to mark as pure. Here we
are defining it just to get the destructor message output.
*/

Ship::~Ship()
{
	cout << "Ship "  << get_name() << " destructed" << endl;
}

bool Ship::can_move() const {
    return is_afloat() && ship_state != State::dead_in_the_water;
}

bool Ship::is_moving() const {
    return ship_state == State::moving_to_position ||
           ship_state == State::moving_to_island ||
           ship_state == State::moving_on_course;
}

bool Ship::is_docked() const {
    return ship_state == State::docked;
}

bool Ship::is_afloat() const {
    return ship_state != State::sunk;
}

bool Ship::can_dock(std::shared_ptr<Island> island_ptr) const { // TODO remove magic numbers
    assert(island_ptr);
    double dist_to_island =
            cartesian_distance(island_ptr->get_location(), get_location());
    if (ship_state != State::stopped || dist_to_island > 0.1)
        return false;
    return true;
}

void Ship::update() {
    if (ship_state == State::sunk) {
        cout << get_name() << " sunk" << endl;
    } else if (is_moving()) {
        calculate_movement();
        cout << get_name() << " now at " << get_location() << endl;
        Model::get_Instance()->notify_location(get_name(), get_location());
    } else if (ship_state == State::stopped) {
        cout << get_name() << " stopped at " << get_location() << endl;
    } else if (is_docked()) {
        cout << get_name() << " docked at " <<
                get_docked_Island()->get_name() << endl;
    } else if (ship_state == State::dead_in_the_water) {
        cout << get_name() << " dead in the water at " << get_location() << endl;
    }
}

void Ship::describe() const {
    cout << get_name() << " at " << get_location();
    if (ship_state == State::sunk)
        cout << " sunk" << endl;
    else {
        cout << ", fuel: " << fuel << " tons, resistance: " << resistance << endl;
        if (ship_state == State::moving_to_position)
            cout << "Moving to " << destination_point <<
                    " on " << track_base.get_course_speed() << endl;
        else if (ship_state == State::moving_to_island)
            cout << "Moving to " << get_destination_Island()->get_name() <<
                    " on " << track_base.get_course_speed() << endl;
        else if (ship_state == State::moving_on_course)
            cout << "Moving on " << track_base.get_course_speed() << endl;
        else if (ship_state == State::docked)
            cout << "Docked at " << get_docked_Island()->get_name() << endl;
        else if (ship_state == State::stopped)
            cout << "Stopped" << endl;
        else if (ship_state == State::dead_in_the_water)
            cout << "Dead in the water" << endl;
    }
}

void Ship::broadcast_current_state() const {
    Model* model_inst = Model::get_Instance();
    model_inst->notify_location(get_name(), get_location());
    model_inst->notify_course(get_name(), track_base.get_course());
    model_inst->notify_speed(get_name(), track_base.get_speed());
    model_inst->notify_fuel(get_name(), fuel);
}

void Ship::set_destination_position_and_speed(Point destination_position,
        double speed) {
    if (speed > get_maximum_speed())
        throw Error("Ship cannot go that fast!");
    if (!can_move())
        throw Error("Ship cannot move!");
    reset_destinations_and_dock();
    Compass_vector vect {get_location(), destination_position};
    set_course(vect.direction);
    set_speed(speed);
    ship_state = State::moving_to_position;
    destination_point = destination_position;
    cout << get_name() << " will sail on " << track_base.get_course_speed() <<
            " to " << destination_position << endl;
}

void Ship::set_destination_island_and_speed(shared_ptr<Island> destination_island,
        double speed) {
    assert(destination_island);
    if (speed > get_maximum_speed())
        throw Error("Ship cannot go that fast!");
    if (!can_move())
        throw Error("Ship cannot move!");
    reset_destinations_and_dock();
    Compass_vector vect {get_location(), destination_island->get_location()};
    set_course(vect.direction);
    set_speed(speed);
    ship_state = State::moving_to_island;
    destination_Island = destination_island;
    destination_point = destination_island->get_location();
    cout << get_name() << " will sail on " << track_base.get_course_speed() <<
            " to " << destination_island->get_name() << endl;
}

void Ship::set_course_and_speed(double course, double speed) {
    if (speed > get_maximum_speed())
        throw Error("Ship cannot go that fast!");
    if (!can_move())
        throw Error("Ship cannot move!");
    reset_destinations_and_dock();
    ship_state = State::moving_on_course;
    set_course(course);
    set_speed(speed);
    cout << get_name() << " will sail on " << track_base.get_course_speed() << endl;
}

void Ship::stop() {
    if (!can_move())
        throw Error("Ship cannot move!");
    reset_destinations_and_dock();
    set_speed(0.);
    ship_state = State::stopped;
    cout << get_name() << " stopping at " << get_location() << endl;
}

void Ship::dock(shared_ptr<Island> island_ptr) {
    assert(island_ptr);
    if (!can_dock(island_ptr))
        throw Error("Can't dock!");
    set_position(island_ptr->get_location());
    docked_Island = island_ptr;
    ship_state = State::docked;
    set_speed(0.);
    cout << get_name() << " docked at " << island_ptr->get_name() << endl;
}

void Ship::refuel() {
    if (!is_docked())
        throw Error("Must be docked!");
    double needed_fuel = fuel_capacity - fuel;
    if (needed_fuel < double_full_gap_c) {
        set_fuel(fuel_capacity);
        return;
    }
    set_fuel(fuel + get_docked_Island()->provide_fuel(needed_fuel));
    cout << get_name() << " now has " << fuel << " tons of fuel" << endl;
}

void Ship::set_load_destination(shared_ptr<Island>) {
    throw Error("Cannot load at a destination!");
}

void Ship::set_unload_destination(shared_ptr<Island>) {
    throw Error("Cannot unload at a destination!");
}

void Ship::attack(shared_ptr<Ship>) {
    throw Error("Cannot attack!");
}

void Ship::stop_attack() {
    throw Error("Cannot attack!");
}

void Ship::receive_hit(int hit_force, shared_ptr<Ship>) {
    resistance -= hit_force;
    cout << get_name() << " hit with " << hit_force << ", resistance now " <<
    resistance << endl;
    if (is_afloat() && resistance < 0.) {
        Model *model_ptr = Model::get_Instance();
        ship_state = State::sunk;
        set_speed(0.);
        cout << get_name() << " sunk" << endl;
        model_ptr->remove_ship(shared_from_this());
        model_ptr->notify_gone(get_name());
    }
}

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
    if ((ship_state == State::moving_to_position ||
         ship_state == State::moving_to_island) &&
        destination_distance <= distance_possible) {
        // yes, make our new position the destination
        set_position(destination_point);
        // we travel the destination distance, using that much fuel
        double fuel_required = destination_distance * fuel_consumption;
        set_fuel(fuel - fuel_required);
        set_speed(0.);
        ship_state = State::stopped;
    }
    else {
        // go as far as we can, stay in the same movement state
        // simply move for the amount of time possible
        track_base.update_position(time_possible);
        // have we used up our fuel?
        if (full_fuel_required >= fuel) {
            set_fuel(0.);
            set_speed(0.);
            ship_state = State::dead_in_the_water;
        }
        else {
            set_fuel(fuel - full_fuel_required);
        }
    }
}

void Ship::reset_destinations_and_dock() {
    destination_point = {0., 0.};
    docked_Island = nullptr;
    destination_Island = nullptr;
}

void Ship::set_position(Point point) {
    track_base.set_position(point);
    Model::get_Instance()->notify_location(get_name(), point);
}

void Ship::set_course(double course) {
    track_base.set_course(course);
    Model::get_Instance()->notify_course(get_name(), course);
}

void Ship::set_speed(double speed) {
    track_base.set_speed(speed);
    Model::get_Instance()->notify_speed(get_name(), speed);
}

void Ship::set_fuel(double fuel_) {
    fuel = fuel_;
    Model::get_Instance()->notify_fuel(get_name(), fuel);
}