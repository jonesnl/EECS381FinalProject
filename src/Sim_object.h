#ifndef SIM_OBJECT_H
#define SIM_OBJECT_H
/* This class provides the interface for all of simulation objects. It also stores the
object's name, and has pure virtual accessor functions for the object's position
and other information. */

#include <string>
struct Point;

class Sim_object {
public:
    // *** define the constructor in Sim_object.cpp to output the supplied message
	Sim_object(const std::string& name_);

    // *** define the destructor in Sim_object.cpp to output the supplied message
    virtual ~Sim_object();
	
	const std::string& get_name() const
		{return name;}
    
	/* Interface for derived classes */
	// *** declare the following as pure virtual functions 
	// ask model to notify views of current state
    virtual void broadcast_current_state() const = 0;
	virtual Point get_location() const = 0;
	virtual void describe() const = 0;
	virtual void update() = 0;
	
	// Sim_objects must be unique, so disable copy/move construction, assignment
    // of base class; this will disable these operations for derived classes also.

private:
	std::string name;
};


#endif
