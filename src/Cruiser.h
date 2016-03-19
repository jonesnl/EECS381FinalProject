#ifndef CRUISER_H
#define CRUISER_H
/*
A Cruiser is a Ship with moderate speed, firepower, and resistance.
When told to attack it will start firing at the target if it is in range.
At each update, it will stop attacking if the target is either no longer afloat
(i.e. is sinking or sunk), or is out of range. As long as the target is both afloat
and in range, it will keep firing at it.

Initial values:
fuel capacity and initial amount: 1000, maximum speed 20., fuel consumption 10.tons/nm, 
resistance 6, firepower 3, maximum attacking range 15
*/

#include "Ship.h"

class Cruiser : public Ship {
public:
    // initialize, then output constructor message
    Cruiser(const std::string &name_, Point position_);

    // output destructor message
    ~Cruiser();

    // perform Cruiser-specific behavior
    void update() override;

    void describe() const override;

    // respond to an attack
    void receive_hit(int hit_force, Ship *attacker_ptr) override;

    // start an attack on a target ship
    void attack(Ship *target_ptr_) override;

    void stop_attack() override;
private:
    int firepower = 3; // Firepower of the curiser
    double attack_range = 15.; // Attack range of the cruiser
    Ship *target = nullptr;    // Target to attack (also controls whether we're
                               // attacking or not
};

#endif