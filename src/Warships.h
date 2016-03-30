#ifndef WARSHIPS_H
#define WARSHIPS_H

#include "Ship.h"

/* A warship has the ability to attack other ships, so it overrides the required
 */
class Warship : public Ship {
public:
    // initialize, then output constructor message
    Warship(const std::string& name_, Point position_, double fuel_capacity_,
            double maximum_speed, double fuel_consumption_, int resistance_, int firepower_,
            double attack_range_);

    // Attack the target if we are attacking during the update.
    // Calls target_out_of_range_handler() if the target is out of range
    void update() override;

    // Describe the warship and whether it is attacking or not
    void describe() const override;

    // Start attacking a target ship
    void attack(std::shared_ptr<Ship> target_ptr_) override;

    // Stop attacking
    void stop_attack() override;
protected:
    // If the target is out of range, derived classes must decide what the ship
    // will do
    virtual void target_out_of_range_handler() = 0;

    // Return true if we are attackign a target
    bool is_attacking() {return attacking;}

    // Return the target of our attack
    std::shared_ptr<Ship> get_attack_target() const {return target.lock();};
private:
    int firepower;          // Firepower of the curiser
    double attack_range;  // Attack range of the cruiser
    bool attacking;
    std::weak_ptr<Ship> target;     // Target to attack (also controls whether we're
                                    // attacking or not
};

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
    void receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr) override;
protected:
    // If we are out of range of our target, move to the target's location so
    // we can attack it when we get in range
    void target_out_of_range_handler() override;
};

#endif