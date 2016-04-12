#include "Grid_location_view.h"

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

// Construct an Grid_location_view using the provided parameters
Grid_location_view::Grid_location_view() :
        y_axis_labels_enabled(true) { }

// Disable drawing y axis labels in the draw() function
void Grid_location_view::disable_y_axis_labels() {
    y_axis_labels_enabled = false;
}

// Track the location of ships
void Grid_location_view::update_location(const string &name, Point location) {
    location_map[name] = location;
}

// Remove the tracking information of a ship
void Grid_location_view::update_remove(const string &name) {
    location_map.erase(name);
}

void Grid_location_view::objects_not_in_grid_handler(
        const std::vector<std::string> &) const
    { }

pair<bool, Point> Grid_location_view::translate_point_handler(Point point) const {
    return {true, point};
}

// Draw the grid
void Grid_location_view::draw(int width, int height, double scale, Point origin) const {
    // Save cout settings since we modify them to draw the map
    auto saved_cout_flags = cout.flags();
    auto saved_precision = cout.precision();

    // We don't want decimal points in our labels
    cout.precision(0);

    vector<vector<string>> grid (height, vector<string>(width, empty_cell_c));
    vector<string> objects_outside_map;

    // Iterate through each location in the location map, adding it to the grid
    // if it maps to a location on the grid. Allow a derived class to translate
    // a point to a new location if they want to.
    for (auto& p : location_map) {
        const string& loc_name = p.first;

        // Translate the tracked point to a new point as specified by derived class
        auto translate_pair = translate_point_handler(p.second);
        if (!translate_pair.first)
            continue; // Ignore point if translate function tells us to

        Point loc_point = translate_pair.second;

        int ix, iy;
        // See if a point maps to a location on the map, if so, add it to the map.
        if (!get_subscripts(ix, iy, loc_point, width, height, scale, origin)) {
            objects_outside_map.push_back(loc_name);
        } else {
            // If more than one object is in the same cell, use the multiple entry cell
            if (grid[iy][ix] == empty_cell_c)
                grid[iy][ix] = loc_name.substr(0, name_abbreviation_length_c);
            else
                grid[iy][ix] = multiple_entry_cell_c;
        }
    }

    // Allow a derived class to do something with the list of objects that are
    // outside the map if they want to.
    objects_not_in_grid_handler(objects_outside_map);

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
            cout << grid[j][i];
        }
        cout << endl;
    }

    // Print the x axis labels
    for (int i = 0; i < width; i += rows_cols_per_label_c) {
        cout << "  " <<
                setw(label_width_c) << origin.x + (i * scale);
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
bool Grid_location_view::get_subscripts(int &ix, int &iy, Point location,
        int width, int height, double scale, Point origin) const {
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

