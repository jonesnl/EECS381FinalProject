#ifndef CRUISER_H
#define CRUISER_H

#include "Warship.h"

/*
 * A Cruiser is a Ship with moderate speed, firepower, and resistance.
 * When told to attack it will start firing at the target if it is in range.
 * At each update, it will stop attacking if the target is either no longer afloat
 * (i.e. is sinking or sunk), or is out of range. As long as the target is both afloat
 * and in range, it will keep firing at it.
*/

class Cruiser : public Warship {
public:
    // initialize the cruiser
    Cruiser(const std::string& name_, Point position_);

    // Add type of ship to description
    void describe() const override;

    // respond to an attack by counter attacking
    void receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr) override;
protected:
    // If the target is out of range, stop the attack
    void target_out_of_range_handler() override;
};

#endif