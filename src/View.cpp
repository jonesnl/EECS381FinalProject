#include "View.h"
#include "Utility.h"

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

const string empty_cell_c = ". ";

/* *** Use this function to calculate the subscripts for the cell. */
View::View () {
    cout << "View constructed" << endl;
	set_defaults();
}

View::~View() {
    cout << "View destructed" << endl;
}

void View::update_location(const std::string &name, Point location) {
    location_map[name] = location;
}

void View::update_remove(const std::string &name) {
    location_map.erase(name);
}

void View::draw() const {
    // Save formatting settings
    auto old_settings = cout.flags();
    auto original_precision = cout.precision();

    vector<vector<string>> array(30, vector<string>(30, empty_cell_c));

    int x, y;
    for (auto& p : location_map) {
        const string& name = p.first;
        Point location = p.second;

        if (get_subscripts(x, y, location)) {
            array[x][y] = name.substr(0, 2); // TODO magic number
        }
    }

    // TODO formatting stuff
    for (auto& int_vect : array) {
        for (auto& str : int_vect) {
            cout << str;
        }
        cout << endl;
    }

    // Restore formatting settings
    cout.flags(old_settings);
    cout.precision(original_precision);
}

void View::set_size(int size) {
    if (size <= 6)
        throw Error("New map size is too small!");
    else if (size > 30)
        throw Error("New map size is too big!");
}

void View::set_scale(double scale_) {
    if (scale_ <= 0.)
        throw Error("New map scale must be positive!");
}

void View::set_origin(Point origin_) {
    origin = origin_;
}

void View::set_defaults() {
	size = 25;
	scale = 2.;
    origin = {-10, -10};
}

// Calculate the cell subscripts corresponding to the supplied location parameter,
// using the current size, scale, and origin of the display.
// This function assumes that origin is a  member variable of type Point,
// scale is a double value, and size is an integer for the number of rows/columns
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool View::get_subscripts(int &ix, int &iy, Point location) const
{
	// adjust with origin and scale
	Cartesian_vector subscripts = (location - origin) / scale;
	// truncate coordinates to appropriate integer after taking the floor
	// floor function will produce the greatest integer that is
	// smaller than the argument, even for negative values.
	// So - 0.05 is floored to -1., which will be outside the array.
	ix = int(floor(subscripts.delta_x));
	iy = int(floor(subscripts.delta_y));
	// if out of range, return false
	if ((ix < 0) || (ix >= size) || (iy < 0) || (iy >= size)) {
		return false;
		}
	else
		return true;
}
