#ifndef TANKER_H
#define TANKER_H
/*
A Tanker is a ship with a large cargo capacity for fuel.
It can be told an Island to load fuel at, and an Island to unload at.
Once it is sent to the loading destination, it will start shuttling between 
the loading and unloading destination. At the loading destination, 
it will first refuel then wait until its cargo hold is full, then it will
go to the unloading destination.

Initial values:
fuel capacity and initial amount 100 tons, maximum speed 10., fuel consumption 2.tons/nm, 
resistance 0, cargo capacity 1000 tons, initial cargo is 0 tons.
*/
#include "Ship.h"

class Island;

class Tanker : public Ship {
public:
	// initialize a tanker
	Tanker(const std::string &name_, Point position_);

	// This class overrides these Ship functions so that it can check if this Tanker has assigned cargo destinations.
	// if so, throw an Error("Tanker has cargo destinations!"); otherwise, simply call the Ship functions.
	void set_destination_position_and_speed(Point destination_point,
			double speed) override;

	void set_destination_island_and_speed(std::shared_ptr<Island> destination_island,
			double speed) override;

	void set_course_and_speed(double course, double speed) override;

	// Set the loading and unloading Island destinations
	// if both cargo destination are already set, throw Error("Tanker has cargo destinations!").
	// if they are the same, leave at the set values, and throw Error("Load and unload cargo destinations are the same!")
	// if both destinations are now set, start the cargo cycle
	void set_load_destination(std::shared_ptr<Island>) override;

	void set_unload_destination(std::shared_ptr<Island>) override;

	// when told to stop, clear the cargo destinations and stop
	void stop() override;

	// perform Tanker-specific behavior
	void update() override;

	// describe the tanker
	void describe() const override;

	double provide_fuel(double request);
private:
    enum class TankerState_t {
        loading, moving_to_unloading, unloading, moving_to_loading, no_cargo_dest
    } tanker_state;
    double cargo; // Current cargo of the tanker
    std::shared_ptr<Island> loading_island; // Loading island location
    std::shared_ptr<Island> unloading_island; // Unloading island location

	// Throw an error if if we are hauling cargo
    void not_hauling_cargo_or_error();
    // Start the tanker cycle if we can
    void start_tanker_cycle_if_possible();
    // Stop the tanker cycle
    void stop_tanker_cycle();
};

#endif