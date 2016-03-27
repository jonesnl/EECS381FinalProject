#include "Utility.h"

#include "Island.h"
#include "Navigation.h"

#include <memory>

using namespace std;

const double double_full_gap_c = 0.005;

IslandDistComp::IslandDistComp(Point location) : common_location(location) {}

bool IslandDistComp::operator() (shared_ptr<Island> i1, shared_ptr<Island> i2) {
    double dist1 = Compass_vector(common_location, i1->get_location()).distance;
    double dist2 = Compass_vector(common_location, i2->get_location()).distance;
    return dist1 < dist2;
}

bool IslandNameComp::operator()(const std::shared_ptr<Island>& p1,
        const std::shared_ptr<Island>& p2){
    return p1->get_name() < p2->get_name();
}

