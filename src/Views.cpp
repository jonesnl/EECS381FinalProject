#include "Views.h"
#include "Utility.h"
#include "Navigation.h"

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <list>
#include <algorithm>
#include <iterator>
#include <cassert>

using namespace std;


/* Shared Helpers */
static bool get_subscripts(int &ix, int &iy, Point location,
        int size, double scale, Point origin);

static void draw_x_axis(int size, double scale, Point origin);

// CoutSettingsSaver saves the format of cout when constructed, then when we leave the
// scope of the object, restore that format. Used in the view draw functions
// if we modify the cout format in order to draw the view.
class CoutSettingsSaver {
public:
    CoutSettingsSaver() :
            flags(cout.flags()), precision(cout.precision()) { }
    ~CoutSettingsSaver() {
        cout.flags(flags);
        cout.precision(precision);
    }
private:
    decltype(cout.flags()) flags;
    decltype(cout.precision()) precision;
};

/* Shared constants */
// Cell strings
const string empty_cell_c = ". ";
const string multiple_entry_cell_c = "* ";
const int rows_cols_per_label_c = 3; // Axis labels are printed every 3 rows/columns
const int label_width_c = 4;
const string empty_y_axis_label = "     ";

/*********** MapView ***********/
const int max_map_size_c = 30;
const int min_map_size_c = 7;
const Point default_map_origin_c = {-10, -10};
const int default_map_size_c = 25;
const double default_map_scale_c = 2.;

// Set the defaults when we construct a new map view
MapView::MapView() : size(default_map_size_c), scale(default_map_scale_c),
        origin(default_map_origin_c) { }

// Add an object to the map, tracking its location
void MapView::update_location(const std::string &name, Point location) {
    location_map[name] = location;
}

// Remove an object from the map
void MapView::update_remove(const std::string &name) {
    location_map.erase(name);
}

// Draw the map
void MapView::draw() const {
    assert("     " == empty_y_axis_label);
    // Save formatting settings
    CoutSettingsSaver settings_saver;
    cout << "Display size: " << size << ", scale: " <<
            scale << ", origin: " << origin << endl;

    // The array that contains the map we will draw
    vector<vector<string>> map_array(max_map_size_c, vector<string>(max_map_size_c, empty_cell_c));
    // The list of objects that are outside of the map
    list<string> objects_outside_map;

    int x, y;
    for (auto& p : location_map) {
        const string& name = p.first;
        Point location = p.second;

        if (get_subscripts(x, y, location, size, scale, origin)) {
            // If multiple objects are in the same cell, use the multiple entry cell,
            // otherwise put the ship's abbreviated name there
            if (map_array[x][y] != empty_cell_c)
                map_array[x][y] = multiple_entry_cell_c;
            else
                map_array[x][y] = name.substr(0, name_abbreviation_length_c);
        } else
            objects_outside_map.push_back(name);
    }

    // Print all the objects outside of the map if we have any
    if (objects_outside_map.size() > 0) {
        for (auto& s : objects_outside_map) {
            if (s != objects_outside_map.back())
                cout << s << ", ";
            else
                cout << s << " outside the map" << endl;
        }
    }

    cout.precision(0);
    // print the map from top to bottom, left to right
    for (int j = size - 1; j >= 0; --j) {
        // Print the label if the row needs one
        if (j % rows_cols_per_label_c == 0) {
            double y_val = origin.y + (j * scale);
            cout << setw(label_width_c) << y_val << " ";
        } else {
            cout << empty_y_axis_label;
        }
        for (int i = 0; i < size; ++i) {
            cout << map_array[i][j];
        }
        cout << endl;
    }

    // Print the column labels
    draw_x_axis(size, scale, origin);
    cout << endl;
}

// Set the map's size
void MapView::set_size(int size_) {
    if (size_ < min_map_size_c)
        throw Error("New map size is too small!");
    else if (size_ > max_map_size_c)
        throw Error("New map size is too big!");
    size = size_;
}

// Set the map's scale
void MapView::set_scale(double scale_) {
    if (scale_ <= 0.)
        throw Error("New map scale must be positive!");
    scale = scale_;
}

// Set the origin of the map
void MapView::set_origin(Point origin_) {
    origin = origin_;
}


void MapView::set_defaults() {
    size = default_map_size_c;
    scale = default_map_scale_c;
    origin = default_map_origin_c;
}

/*********** SailingView **********/
const int sailing_column_width_c = 10;

// Update the course of a ship in the data structure
void SailingView::update_course(const string &name, double course) {
    sailing_data_map[name].course = course;
}

// Update the speed of a ship in the data structure
void SailingView::update_speed(const string &name, double speed) {
    sailing_data_map[name].speed = speed;
}

// Update the fuel of a ship in the data structure
void SailingView::update_fuel(const string& name, double fuel) {
    sailing_data_map[name].fuel = fuel;
}

// Remove a ship from the data structure
void SailingView::update_remove(const string& name) {
    sailing_data_map.erase(name);
}

// Draw the sailing statistics of all ships on the ocean
void SailingView::draw() const {
    // Print the header
    cout << "----- Sailing Data -----" << endl;
    cout << setw(sailing_column_width_c) << "Ship" <<
            setw(sailing_column_width_c) << "Fuel" <<
            setw(sailing_column_width_c) << "Course" <<
            setw(sailing_column_width_c) << "Speed" << endl;

    // Print the data
    for(auto& data_pair : sailing_data_map) {
        cout << setw(sailing_column_width_c) << data_pair.first;
        const SailingData& sailing_data = data_pair.second;
        cout << setw(sailing_column_width_c) << sailing_data.fuel <<
                setw(sailing_column_width_c) << sailing_data.course <<
                setw(sailing_column_width_c) << sailing_data.speed <<
                endl;
    }
}

/******* BridgeView *******/
// Some constants used for printing the view
const Point bridge_view_origin_c = {-90., 0.};
const double bridge_view_scale_c = 10.;
const int bridge_view_size_c = 19;
const int bridge_view_sky_height_c = 2;
const double bridge_view_draw_dist_c = 20.;

// Construct a bridge view for a ship called ship_name_
BridgeView::BridgeView(const string& ship_name_) :
        ownship_name(ship_name_), sunk(false) { }

// Update the location of objects on the ocean
void BridgeView::update_location(const string& name, Point point) {
    if (sunk)
        return;
    if (ownship_name == name)
        ownship_location = point;
    else
        location_map[name] = point;
}

// Update the course of ownship
void BridgeView::update_course(const string &name, double course) {
    if (sunk)
        return;
    if (name == ownship_name)
        ownship_heading = course;
}

// Remove objects from the ocean if they sink. If we are removed, that means
// that ownship has sunk
void BridgeView::update_remove(const string& name) {
    if (sunk)
        return;
    if (name == ownship_name)
        sunk = true;
    else
        location_map.erase(name);
}

// Draw the view from the bridge of the ship
void BridgeView::draw() const {
    CoutSettingsSaver settings_saver;
    if (sunk) {
        // If the ship is sunk, use a special bridge view that shows only water
        cout << "Bridge view from " << ownship_name <<
                " sunk at " << ownship_location << endl;
        for (int i = 0; i < bridge_view_sky_height_c + 1; ++i) {
            cout << "     ";
            for (int j = 0; j < bridge_view_size_c; ++j)
                cout << "w-";
            cout << endl;
        }
    } else {
        // If we're afloat, print information about ownship
        cout << "Bridge view from " << ownship_name <<
                " position " << ownship_location <<
                " heading " << ownship_heading << endl;

        // Print the sky
        for (int i = 0; i < bridge_view_sky_height_c; ++i) {
            cout << empty_y_axis_label;
            for (int j = 0; j < bridge_view_size_c; ++j)
                cout << empty_cell_c;
            cout << endl;
        }

        // Construct the view_array which will show what is in any given 10 degree
        // chunk of our view off the front of the bridge of our ship
        vector<string> view_array(bridge_view_size_c, empty_cell_c);
        for (const auto& name_loc_pair : location_map) {
            Compass_position position(ownship_location, name_loc_pair.second);
            // If the object is out of range, do not show it to the user
            if (position.range > bridge_view_draw_dist_c ||
                    position.range < double_close_enough_c)
                continue;

            // Figure out the angle off the bow the target object is at
            double bow_angle = position.bearing - ownship_heading;
            // Make the resulting bow_angle between -180 and 180 degrees
            if (bow_angle > 180.)
                bow_angle -= 360.;
            else if (bow_angle < -180.)
                bow_angle += 360.;

            int ix, dummy;
            // If the resulting bow_angle is not within -90 to 100 degrees, do not
            // add it to the view. ix tells us what cell of the view we should put
            // the object in.
            if (!get_subscripts(ix, dummy, {bow_angle, 0}, bridge_view_size_c,
                    bridge_view_scale_c, bridge_view_origin_c))
                continue;

            // If more than one object is in the same cell, use the multiple entry cell
            if (view_array[ix] == empty_cell_c)
                view_array[ix] = name_loc_pair.first.substr(0, name_abbreviation_length_c);
            else
                view_array[ix] = multiple_entry_cell_c;
        }

        // Print the view array to the screen
        cout << empty_y_axis_label;
        copy(view_array.cbegin(), view_array.cend(), ostream_iterator<string>(cout));
        cout << endl;
    }

    // Print the x axis labels
    cout.precision(0);
    draw_x_axis(bridge_view_size_c, bridge_view_scale_c, bridge_view_origin_c);
    cout << endl;

}

/********* Shared Helpers **********/

// Calculate the cell subscripts corresponding to the supplied location parameter,
// using the current size, scale, and origin of the display.
// This function assumes that origin is a  member variable of type Point,
// scale is a double value, and size is an integer for the number of rows/columns
// currently being used for the grid.
// Return true if the location is within the grid, false if not
static bool get_subscripts(int &ix, int &iy, Point location,
        int size, double scale, Point origin) {
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

// Draw the x axis for both the MapView and the BridgeView
static void draw_x_axis(int size, double scale, Point origin) {
    for (int i = 0; i < size; i += rows_cols_per_label_c) {
        cout << "  " << setw(label_width_c) << origin.x + (i * scale);
    }
}
