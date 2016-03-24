#include "Views.h"
#include "Geometry.h"
#include "Utility.h"

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <list>

using namespace std;

const string empty_cell_c = ". ";
const int sailing_column_width_c = 10;

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

MapView::MapView () {
    cout << "View constructed" << endl;
    set_defaults();
}

MapView::~MapView() {
    cout << "View destructed" << endl;
}

void MapView::update_location(const std::string &name, Point location) {
    location_map[name] = location;
}

void MapView::update_remove(const std::string &name) {
    location_map.erase(name);
}

void MapView::draw() const {
    // Save formatting settings
    CoutSettingsSaver settings_saver;
    cout << "Display size: " << size << ", scale: " <<
    scale << ", origin: " << origin << endl;

    vector<vector<string>> array(30, vector<string>(30, empty_cell_c));
    list<string> objects_outside_map;
    int x, y;
    for (auto& p : location_map) {
        const string& name = p.first;
        Point location = p.second;

        if (get_subscripts(x, y, location)) {
            if (array[x][y] != empty_cell_c)
                array[x][y] = "* ";
            else
                array[x][y] = name.substr(0, 2); // TODO magic number
        } else
            objects_outside_map.push_back(name);
    }

    if (objects_outside_map.size() > 0) {
        for (auto& s : objects_outside_map) {
            if (s != objects_outside_map.back())
                cout << s << ", ";
            else
                cout << s << " outside the map" << endl;
        }
    }

    cout.precision(0);
    for (int j = size - 1; j >= 0; --j) {
        if (j % 3 == 0) {
            double y_val = origin.y + (j * scale);
            cout << setw(4) << y_val << " ";
        } else {
            cout << "     ";
        }
        for (int i = 0; i < size; ++i) {
            cout << array[i][j];
        }
        cout << endl;
    }
    for (int i = 0; i < size; i += 3) {
        cout << "  " << setw(4) << origin.x + (i * scale);
    }
    cout << endl;
}

void MapView::set_size(int size_) {
    if (size_ <= 6)
        throw Error("New map size is too small!");
    else if (size_ > 30)
        throw Error("New map size is too big!");
    size = size_;
}

void MapView::set_scale(double scale_) {
    if (scale_ <= 0.)
        throw Error("New map scale must be positive!");
    scale = scale_;
}

void MapView::set_origin(Point origin_) {
    origin = origin_;
}

void MapView::set_defaults() {
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
bool MapView::get_subscripts(int &ix, int &iy, Point location) const
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

void SailingView::update_course_speed(const std::string& name, Course_speed cs) {
    sailing_data_map[name].course_speed = cs;
}

void SailingView::update_fuel(const std::string& name, double fuel) {
    sailing_data_map[name].fuel = fuel;
}

void SailingView::update_remove(const std::string& name) {
    sailing_data_map.erase(name);
}

void SailingView::draw() const {
    CoutSettingsSaver settings_saver;
    cout << "----- Sailing Data -----" << endl;
    cout << setw(sailing_column_width_c) << "Ship" <<
            setw(sailing_column_width_c) << "Fuel" <<
            setw(sailing_column_width_c) << "Course" <<
            setw(sailing_column_width_c) << "Speed" << endl;
    for(auto& data_pair : sailing_data_map) {
        cout << setw(sailing_column_width_c) << data_pair.first;
        const SailingData& sailing_data = data_pair.second;
        cout << setw(sailing_column_width_c) << sailing_data.fuel <<
                setw(sailing_column_width_c) << sailing_data.course_speed.course <<
                setw(sailing_column_width_c) << sailing_data.course_speed.speed <<
                endl;
    }
}

