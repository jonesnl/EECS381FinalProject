#include "Utility.h"

#include "Island.h"
#include "Navigation.h"

using namespace std;

/* Shared constants */
const double double_close_enough_c = 0.005;
const size_t name_abbreviation_length_c = 2;

/* DistComp */
// Construct a distance comparator object
DistComp::DistComp(Point location) : common_loc(location) {}

// Compare which object is closer to the common_location
bool DistComp::operator() (const shared_ptr<Sim_object>& i1,
        const shared_ptr<Sim_object>& i2) {
    double dist1 = Compass_vector(common_loc, i1->get_location()).distance;
    double dist2 = Compass_vector(common_loc, i2->get_location()).distance;
    return dist1 < dist2;
}

/* NameComp */
// Compare the names of two objects
bool NameComp::operator()(const std::shared_ptr<Sim_object>& p1,
        const std::shared_ptr<Sim_object>& p2){
    return p1->get_name() < p2->get_name();
}

// Compare a string with an object's name
bool NameComp::operator() (const std::string& name1,
        const std::shared_ptr<Sim_object>& p2) {
    return name1 < p2->get_name();
}

// Compare an object's name with a string
bool NameComp::operator() (const std::shared_ptr<Sim_object>& p1,
        const std::string& name2) {
    return p1->get_name() < name2;
}
