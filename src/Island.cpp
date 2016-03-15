#include "Island.h"

#include "Model.h"

#include <iostream>

using std::cout; using std::endl;

Island::Island(const std::string &name_, Point position_, double fuel_,
        double production_rate_) : Sim_object(name_), position(position_),
        fuel(fuel_), production_rate(production_rate_) {

    cout << "Island " << get_name() << " constructed" << endl;
}

Island::~Island() {
    cout << "Island " << get_name() << " destructed" << endl;
}

void Island::update() {
    // Production rate is assumed to be on a per hour basis
    if (production_rate > 0.)
        accept_fuel(production_rate);
}

void Island::describe() const {
    cout << "\nIsland " << get_name() << " at position " << position << endl;
    cout << "Fuel available: " << fuel << " tons" << endl;
}

void Island::broadcast_current_state() const {
    g_Model_ptr->notify_location(get_name(), position);
}

double Island::provide_fuel(double request) {
    if (request > fuel)
        request = fuel;
    fuel -= request;
    cout << "Island " << get_name() << " supplied " <<
            request << " tons of fuel" << endl;
    return request;
}

void Island::accept_fuel(double amount) {
    fuel += amount;
    cout << "Island " << get_name() << " now has " << fuel << " tons" << endl;
}
