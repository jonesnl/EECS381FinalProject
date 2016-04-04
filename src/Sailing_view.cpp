#include "Sailing_view.h"

#include <iostream>
#include <iomanip>

using namespace std;

const int sailing_column_width_c = 10;

// Update the course of a ship in the data structure
void Sailing_view::update_course(const string &name, double course) {
    sailing_data_map[name].course = course;
}

// Update the speed of a ship in the data structure
void Sailing_view::update_speed(const string &name, double speed) {
    sailing_data_map[name].speed = speed;
}

// Update the fuel of a ship in the data structure
void Sailing_view::update_fuel(const string& name, double fuel) {
    sailing_data_map[name].fuel = fuel;
}

// Remove a ship from the data structure
void Sailing_view::update_remove(const string& name) {
    sailing_data_map.erase(name);
}

// Draw the sailing statistics of all ships on the ocean
void Sailing_view::draw() const {
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

