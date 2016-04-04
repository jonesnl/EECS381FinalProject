#include "Ocean_map.h"

#include "Utility.h"

#include <cassert>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

/* map is indexed using map[y][x] instead of map[x][y]. */

const string empty_cell_c = ". ";
const string multiple_entry_cell_c = "* ";
const int rows_cols_per_label_c = 3; // Axis labels are printed every 3 rows/columns
const int label_width_c = 4;
const string empty_y_axis_label = "     ";

// Construct an Ocean_map using the provided parameters
Ocean_map::Ocean_map(int width_, int height_, double scale_, Point origin_) :
        width(width_), height(height_),
        map(height, vector<string>(width, empty_cell_c)), scale(scale_),
        origin(origin_), y_axis_labels_enabled(true) { }

// Add an object to the map
bool Ocean_map::add_to_map(const string& obj_name, Point point) {
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
void Ocean_map::fill(const string& fill_string) {
    assert(fill_string.length() == name_abbreviation_length_c);
    for (auto& row_vect : map) {
        for (auto& str : row_vect) {
            str = fill_string;
        }
    }
}

// Disable drawing y axis labels in the draw() function
void Ocean_map::disable_y_axis_labels() {
    y_axis_labels_enabled = false;
}

// Draw the ocean
void Ocean_map::draw() const {
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
bool Ocean_map::get_subscripts(int &ix, int &iy, Point location) {
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

