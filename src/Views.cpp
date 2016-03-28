#include "Views.h"
#include "Geometry.h"
#include "Utility.h"
#include "Navigation.h"

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <list>
#include <cassert>
#include <algorithm>
#include <iterator>

using namespace std;


const int sailing_column_width_c = 10;

/* Shared Helpers */
const string empty_cell_c = ". ";
static bool get_subscripts(int &ix, int &iy, Point location,
        int size, double scale, Point origin);

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

/*********** MapView ***********/
MapView::MapView () {
    set_defaults();
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

        if (get_subscripts(x, y, location, size, scale, origin)) {
            if (array[x][y] != empty_cell_c)
                array[x][y] = "* ";
            else
                array[x][y] = name_abrv(name);
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
    origin = Point {-10, -10};
}

/*********** SailingView **********/
void SailingView::update_course(const string &name, double course) {
    sailing_data_map[name].course = course;
}

void SailingView::update_speed(const string &name, double speed) {
    sailing_data_map[name].speed = speed;
}

void SailingView::update_fuel(const string& name, double fuel) {
    sailing_data_map[name].fuel = fuel;
}

void SailingView::update_remove(const string& name) {
    sailing_data_map.erase(name);
}

void SailingView::draw() const {
    cout << "----- Sailing Data -----" << endl;
    cout << setw(sailing_column_width_c) << "Ship" <<
            setw(sailing_column_width_c) << "Fuel" <<
            setw(sailing_column_width_c) << "Course" <<
            setw(sailing_column_width_c) << "Speed" << endl;
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

BridgeView::BridgeView(const string& ship_name_) :
        ownship(ship_name_) { }

void BridgeView::update_location(const string& name, Point point) {
    if (name == ownship)
        ship_location = point;
    else
        location_map[name] = point;
}

void BridgeView::update_course(const string &name, double course) {
    if (name == ownship)
        ship_heading = course;
}

void BridgeView::update_remove(const string& name) {
    if (name == ownship)
        sunk = true;
    else
        location_map.erase(name);
}

void BridgeView::draw() const {
    CoutSettingsSaver settings_saver;
    if (sunk) {
        cout << "Bridge view from " << ownship <<
        " sunk at " << ship_location << endl;
        for (int i = 0; i < 3; ++i) {
            cout << "     ";
            for (int j = 0; j < 19; ++j)
                cout << "w-";
            cout << endl;
        }
    } else {
        cout << "Bridge view from " << ownship <<
        " position " << ship_location <<
        " heading " << ship_heading << endl;
        for (int i = 0; i < 2; ++i) {
            cout << "     ";
            for (int j = 0; j < 19; ++j)
                cout << empty_cell_c;
            cout << endl;
        }

        vector<string> array(19, empty_cell_c);// TODO name
        for (const auto& name_loc_pair : location_map) {
            Compass_position position(ship_location, name_loc_pair.second);
            if (position.range > 20 || position.range < double_close_enough_c)
                continue;
            int ix, dummy;
            double bow_angle = position.bearing - ship_heading;
            if (bow_angle > 180.)
                bow_angle -= 360.;
            else if (bow_angle < -180.)
                bow_angle += 360.;

            if (!get_subscripts(ix, dummy, {bow_angle, 0}, 19, 10.,
                    {-90., 0.}))
                continue;
            if (array[ix] == empty_cell_c)
                array[ix] = name_abrv(name_loc_pair.first);
            else
                array[ix] = "* "; // TODO magic value
        }
        cout << "     ";
        copy(array.cbegin(), array.cend(), ostream_iterator<string>(cout));
        cout << endl;
    }
    cout.precision(0);
    for (int i = 0; i < 19; i += 3) { // TODO duplicated code
        cout << "  " << setw(4) << -90. + (i * 10.);
    }
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
