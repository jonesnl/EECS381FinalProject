#include "Map_view.h"

#include "Ocean_map.h"
#include "Utility.h"

#include <iostream>

using namespace std;

const int max_map_size_c = 30;
const int min_map_size_c = 7;
const Point default_map_origin_c = {-10, -10};
const int default_map_size_c = 25;
const double default_map_scale_c = 2.;

// Set the defaults when we construct a new map view
Map_view::Map_view() : size(default_map_size_c), scale(default_map_scale_c),
                     origin(default_map_origin_c) { }

// Add an object to the map, tracking its location
void Map_view::update_location(const std::string &name, Point location) {
    location_map[name] = location;
}

// Remove an object from the map
void Map_view::update_remove(const std::string &name) {
    location_map.erase(name);
}

// Draw the map
void Map_view::draw() const {
    cout << "Display size: " << size << ", scale: " <<
    scale << ", origin: " << origin << endl;

    Ocean_map map(size, size, scale, origin);

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

// Set the origin of the map
void Map_view::set_origin(Point origin_) {
    origin = origin_;
}


void Map_view::set_defaults() {
    size = default_map_size_c;
    scale = default_map_scale_c;
    origin = default_map_origin_c;
}

