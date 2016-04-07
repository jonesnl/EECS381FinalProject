#ifndef VIEW_H
#define VIEW_H
/* *** View Class (Interface) ***
 * The View class represents the interface of derived views. All update_* functions
 * are by default implemented as empty functions so that derived classes only have
 * to override update_* functions for data that the view is interested in. If the
 * derived view is not interested in a data type, it does not have to override the
 * function and that data will be ignored in that view.
 *
 * The draw() function must be implemented in each derived View class, as it is
 * what is called when someone wants to draw the view.
 */

#include "Geometry.h"

class View {
public:
    // Virtual destructor to make sure right destructors are called.
	virtual ~View() = default;

    // Update the location of an object in the view.
	virtual void update_location(const std::string& name, Point location);

    // Update the course of an object in the view.
	virtual void update_course(const std::string& name, double course);

    // Update the speed of an object in the view.
	virtual void update_speed(const std::string& name, double speed);

    // Update the fuel of an object in the view.
    virtual void update_fuel(const std::string& name, double fuel);

	// Remove an object's data from a view; no error if the object is not in the view
	virtual void update_remove(const std::string& name);
	
	// Draw the view.
	virtual void draw() const = 0;
};

#endif
