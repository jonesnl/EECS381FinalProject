#include "Views.h"
#include "Utility.h"
#include "Navigation.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>
#include <list>
#include <algorithm>
#include <iterator>
#include <cassert>

using namespace std;

/* OceanMap Helper Class Declaration
 *
 * An OceanMap is used to create and draw a map view of the ocean. You set the maps
 * width, height, scale, and origin as constructor elements, then you add locations
 * to the map. If a cell in the map is filled by more than one object, it will
 * use "* " as the cell. If no object is in the map, it will use ". " for the cell.
 * If only one object is in a cell, it will use an abbreviation of the string as
 * the cell contents "Aj".
 *
 * You can fill the map with a specific string if you wish using the fill() function.
 *
 * Draw the map using the draw function.
 */
class OceanMap {
public:
    // Construct an OceanMap object
    OceanMap(int width_, int height_, double scale_, Point origin_);

    // Returns false when the object we are trying to add to the map is not added,
    // due to it lying outside the bounds of the map. Otherwise the object's name
    // is abbreviated and added to the map at the cell corresponding to "point" and
    // the function returns true.
    bool add_to_map(const string& obj_name, Point point);

    // Fill the map with a given character string.
    void fill(const string& fill_string);

    // Disable drawing the y axis when the draw() function is called. By default
    // OceanMap will draw y axis labels.
    void disable_y_axis_labels();

    // Draw the map.
    void draw() const;
private:
    int width, height; // Width and height of the map
    vector<vector<string>> map; // Cell elements are contained in the 2-D map array.
    double scale; // Scale of the map
    Point origin; // Origin of the map
    bool y_axis_labels_enabled; // Controls whether we draw y axis labels.

    // Helper function that maps a point to a specific cell in the map. Returns
    // false if the location does not correspond to a map cell.
    bool get_subscripts(int &ix, int &iy, Point location);
};

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
    cout << "Display size: " << size << ", scale: " <<
            scale << ", origin: " << origin << endl;

    OceanMap map(size, size, scale, origin);

    // The list of objects that are outside of the map
    vector<string> objects_outside_map;
    for (auto& p : location_map) {
        if (!map.add_to_map(p.first, p.second))
            objects_outside_map.push_back(p.first);
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

    // Draw map with y axis labels
    map.draw();
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
const int bridge_view_map_width_c = 19;
const int bridge_view_map_height_c = 3;
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
    OceanMap ocean_map(bridge_view_map_width_c, bridge_view_map_height_c,
            bridge_view_scale_c, bridge_view_origin_c);
    if (sunk) {
        // If the ship is sunk, use a special bridge view that shows only water
        cout << "Bridge view from " << ownship_name <<
                " sunk at " << ownship_location << endl;
        ocean_map.fill("w-");
    } else {
        // If we're afloat, print information about ownship
        cout << "Bridge view from " << ownship_name <<
                " position " << ownship_location <<
                " heading " << ownship_heading << endl;

        // Construct the view_array which will show what is in any given 10 degree
        // chunk of our view off the front of the bridge of our ship
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

            // If the resulting bow_angle is not within -90 to 100 degrees, do not
            // add it to the view. ix tells us what cell of the view we should put
            // the object in.
            ocean_map.add_to_map(name_loc_pair.first, {bow_angle, 0});
        }
    }
    ocean_map.disable_y_axis_labels();
    ocean_map.draw();
}

/* OceanMap Implementation */

/* map is indexed using map[y][x] instead of map[x][y]. */

const string empty_cell_c = ". ";
const string multiple_entry_cell_c = "* ";
const int rows_cols_per_label_c = 3; // Axis labels are printed every 3 rows/columns
const int label_width_c = 4;
const string empty_y_axis_label = "     ";

// Construct an OceanMap using the provided parameters
OceanMap::OceanMap(int width_, int height_, double scale_, Point origin_) :
        width(width_), height(height_),
        map(height, vector<string>(width, empty_cell_c)), scale(scale_),
        origin(origin_), y_axis_labels_enabled(true) { }

// Add an object to the map
bool OceanMap::add_to_map(const string& obj_name, Point point) {
    int ix, iy;
    // See if a point maps to a location on the map, if so, add it to the map.
    if (!get_subscripts(ix, iy, point))
        return false;

    // If more than one object is in the same cell, use the multiple entry cell
    if (map[iy][ix] == empty_cell_c)
        map[iy][ix] = obj_name.substr(0, name_abbreviation_length_c);
    else
        map[iy][ix] = multiple_entry_cell_c;
    return true;
}

// Fill the map with the provided string
void OceanMap::fill(const string& fill_string) {
    assert(fill_string.length() == name_abbreviation_length_c);
    for (auto& row_vect : map) {
        for (auto& str : row_vect) {
            str = fill_string;
        }
    }
}

// Disable drawing y axis labels in the draw() function
void OceanMap::disable_y_axis_labels() {
    y_axis_labels_enabled = false;
}

// Draw the ocean
void OceanMap::draw() const {
    // Save cout settings since we modify them to draw the map
    auto saved_cout_flags = cout.flags();
    auto saved_precision = cout.precision();

    // We don't want decimal points in our labels
    cout.precision(0);

    // Print the map from top to bottom, left to right
    for (int j = height - 1; j >= 0; --j) {
        // Print the label if the row needs one
        if (y_axis_labels_enabled && j % rows_cols_per_label_c == 0) {
            double y_val = origin.y + (j * scale);
            cout << setw(label_width_c) << y_val << " ";
        } else {
            cout << empty_y_axis_label;
        }
        // Print the row in the map
        for (int i = 0; i < width; ++i) {
            cout << map[j][i];
        }
        cout << endl;
    }

    // Print the x axis labels
    for (int i = 0; i < width; i += rows_cols_per_label_c) {
        cout << "  " << setw(label_width_c) << origin.x + (i * scale);
    }
    cout << endl;

    // Restore the cout settings
    cout.flags(saved_cout_flags);
    cout.precision(saved_precision);
}

// Calculate the cell subscripts corresponding to the supplied location parameter,
// using the current size, scale, and origin of the display.
// This function assumes that origin is a  member variable of type Point,
// scale is a double value, and size is an integer for the number of rows/columns
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool OceanMap::get_subscripts(int &ix, int &iy, Point location) {
    // adjust with origin and scale
    Cartesian_vector subscripts = (location - origin) / scale;
    // truncate coordinates to appropriate integer after taking the floor
    // floor function will produce the greatest integer that is
    // smaller than the argument, even for negative values.
    // So - 0.05 is floored to -1., which will be outside the array.
    ix = int(floor(subscripts.delta_x));
    iy = int(floor(subscripts.delta_y));
    // if out of range, return false
    if ((ix < 0) || (ix >= width) || (iy < 0) || (iy >= height)) {
        return false;
    }
    else
        return true;
}

