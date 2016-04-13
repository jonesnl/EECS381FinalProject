#ifndef TORPEDO_BOAT_H
#define TORPEDO_BOAT_H

#include "Warship.h"

/* A Torpedo_boat is a short range warship that can follow a boat while it is attacking,
 * and will take evasive action if it is attacked by retreating to a nearby island.
 * The island we will retreat to is the closest island to the attacker that is at
 * least 15 km from the attacker.
 */

class Torpedo_boat : public Warship {
public:
    // Construct a torpedo boat
    Torpedo_boat(const std::string& name_, Point position_);

    // Add the ship type to the description
    void describe() const override;

    // If we are hit, take evasive action
    void receive_hit(int hit_force, std::shared_ptr<Ship_component> attacker_ptr) override;
protected:
    // If we are out of range of our target, move to the target's location so
    // we can attack it when we get in range
    void target_out_of_range_handler() override;
};

#endif