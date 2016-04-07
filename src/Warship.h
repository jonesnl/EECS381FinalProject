#ifndef WARSHIPS_H
#define WARSHIPS_H

#include "Ship.h"

/* A warship has the ability to attack other ships, so it overrides the required
 */
class Warship : public Ship {
public:
    // initialize a warship
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
    // Derived classes must decide what to do if the target is out of range durring
    // an attack
    virtual void target_out_of_range_handler() = 0;

    // Return true if we are attacking a target
    bool is_attacking() {return attacking;}

    // Return the target of our attack
    std::shared_ptr<Ship> get_attack_target() const {return target.lock();};
private:
    int firepower;          // Firepower of the curiser
    double attack_range;  // Attack range of the cruiser
    bool attacking;
    std::weak_ptr<Ship> target;     // Target to attack
};


#endif