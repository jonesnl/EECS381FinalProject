#include "Map_view.h"

#include "Grid_location_view.h"
#include "Utility.h"

#include <iostream>

using namespace std;

const int max_map_size_c = 30;
const int min_map_size_c = 7;
const Point default_map_origin_c = {-10, -10};
const int default_map_size_c = 25;
const double default_map_scale_c = 2.;

// Set the defaults when we construct a new map view
Map_view::Map_view() :
       size(default_map_size_c), scale(default_map_scale_c),
       origin(default_map_origin_c) { }

// Set the map's size
void Map_view::set_size(int size_) {
    if (size_ < min_map_size_c)
        throw Error("New map size is too small!");
    else if (size_ > max_map_size_c)
        throw Error("New map size is too big!");
    size = size_;
}

// Set the map's scale
void Map_view::set_scale(double scale_) {
    if (scale_ <= 0.)
        throw Error("New map scale must be positive!");
    scale = scale_;
}

void Map_view::set_origin(Point origin_) {
    origin = origin_;
}


void Map_view::set_defaults() {
    set_size(default_map_size_c);
    set_scale(default_map_scale_c);
    set_origin(default_map_origin_c);
}


// Draw the map
void Map_view::draw() const {
    cout << "Display size: " << size << ", scale: " <<
            scale << ", origin: " << origin << endl;

    Grid_location_view::draw(size, size, scale, origin);
    return;
}

// If some objects are not in the grid
void Map_view::objects_not_in_grid_handler(
        const vector<string> &objects_outside_map) const {
    // Print all the objects outside of the map if we have any
    if (objects_outside_map.size() > 0) {
        for (auto& s : objects_outside_map) {
            if (s != objects_outside_map.back())
                cout << s << ", ";
            else
                cout << s << " outside the map" << endl;
        }
    }
}
