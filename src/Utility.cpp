#include "Utility.h"

#include "Island.h"
#include "Navigation.h"

using namespace std;

/* Shared constants */
const double double_close_enough_c = 0.005;
const size_t name_abbreviation_length_c = 2;

/* IslandDistComp */
// Construct a distance comparator object
IslandDistComp::IslandDistComp(Point location) : common_location(location) {}

// Compare which island is closer to the common_location
bool IslandDistComp::operator() (shared_ptr<Island> i1, shared_ptr<Island> i2) {
    double dist1 = Compass_vector(common_location, i1->get_location()).distance;
    double dist2 = Compass_vector(common_location, i2->get_location()).distance;
    return dist1 < dist2;
}

/* IslandNameComp */
// Compare the names of two islands
bool IslandNameComp::operator()(const std::shared_ptr<Island>& p1,
        const std::shared_ptr<Island>& p2){
    return p1->get_name() < p2->get_name();
}
