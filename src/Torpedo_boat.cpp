#include "Torpedo_boat.h"

#include "Model.h"
#include "Utility.h"
#include "Island.h"

#include <iostream>
#include <cassert>
#include <algorithm>

using namespace std;

const double torpedo_boat_fuel_capacity_c = 800.;
const double torpedo_boat_maximum_speed_c = 12.;
const double torpedo_boat_fuel_consumption_c = 5.;
const int torpedo_boat_resistance_c = 9;
const int torpedo_boat_firepower_c = 3;
const double torpedo_boat_attack_range_c = 5.;
const double torpedo_boat_retreat_dist_c = 15.;

// Construct a Torpedo_boat
Torpedo_boat::Torpedo_boat(const string& name_, Point position_) :
        Warship(name_, position_, torpedo_boat_fuel_capacity_c,
                torpedo_boat_maximum_speed_c,
                torpedo_boat_fuel_consumption_c,
                torpedo_boat_resistance_c,
                torpedo_boat_firepower_c,
                torpedo_boat_attack_range_c) { }

// Add a ship type to the description
void Torpedo_boat::describe() const {
    cout << "\nTorpedo_boat ";
    Warship::describe();
}

// If we are hit, and we can move, take evasive action by retreating to the nearest
// island to the attacker that is at least torpedo_boat_retreat_dist_c away from the
// attacker. This boat does not counter attack.
void Torpedo_boat::receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr) {
    Warship::receive_hit(hit_force, attacker_ptr);
    if (can_move()) {
        cout << get_name() << " taking evasive action" << endl;
        if (is_attacking())
            stop_attack();

        // Take evasive action
        // Vector is sorted by island name by default
        auto island_vect = Model::get_inst()->get_vector_of_islands();
        assert(island_vect.size() > 0);

        Point attacker_position = attacker_ptr->get_location();
        // Sort by distance from the attacker, closest first
        stable_sort(island_vect.begin(), island_vect.end(),
                IslandDistComp{attacker_position});

        // Find closest island that is at least 15 nm from the attacker
        auto dest_itt = find_if(island_vect.begin(), island_vect.end(),
                [attacker_position](shared_ptr<Island> isl) {
                    return Compass_vector(attacker_position,
                            isl->get_location()).distance >= torpedo_boat_retreat_dist_c;
                });
        // If no island at least 15 nm from attacker, choose island that is
        // furthest from the attacker
        if (dest_itt == island_vect.end()) {
            // Can't just take the last element of the array since we still
            // need to take the name into account if the last couple islands
            // are the same distance from each other.
            dest_itt = max_element(island_vect.begin(), island_vect.end(),
                    IslandDistComp{attacker_position});
        }
        assert(dest_itt != island_vect.end());
        set_destination_island_and_speed(*dest_itt, get_maximum_speed());
    }
}

// If our target is out of range, and we can move, move towards the target's location
void Torpedo_boat::target_out_of_range_handler() {
    if (can_move())
        set_destination_position_and_speed(get_attack_target()->get_location(),
                get_maximum_speed());
}
