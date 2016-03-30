#ifndef SIM_OBJECT_H
#define SIM_OBJECT_H
/* This class provides the interface for all of simulation objects. It also stores the
object's name, and has pure virtual accessor functions for the object's position
and other information. */

#include <string>
struct Point;

class Sim_object {
public:
	// Sim_object constructor
	Sim_object(const std::string& name_);

	// Use the default destructor for the Sim_object. Needs to be virtual
	// so derived classes can override it.
    virtual ~Sim_object() = default;
	
	const std::string& get_name() const
		{return name;}
    
	/* Interface for derived classes */
	// ask model to notify views of current state
    virtual void broadcast_current_state() const = 0;
	virtual Point get_location() const = 0;
	virtual void describe() const = 0;
	virtual void update() = 0;
	
	// Sim_objects must be unique, so disable copy/move construction, assignment
    // of base class; this will disable these operations for derived classes also.
	Sim_object(const Sim_object&) = delete;
	Sim_object(const Sim_object&&) = delete;
	Sim_object& operator= (const Sim_object&) = delete;
	Sim_object& operator= (const Sim_object&&) = delete;

private:
	std::string name; // The name of the object
};


#endif
