#ifndef WARSHIPS_H
#define WARSHIPS_H
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

class Warship : public Ship {
public:
    // initialize, then output constructor message
    Warship(const std::string& name_, Point position_, double fuel_capacity_,
            double maximum_speed, double fuel_consumption_, int resistance_, int firepower_,
            double attack_range_);

    // output destructor message
    ~Warship();

    // perform Cruiser-specific behavior
    void update() override;

    void describe() const override;

    // start an attack on a target ship
    void attack(std::shared_ptr<Ship> target_ptr_) override;

    void stop_attack() override;
protected:
    virtual void target_out_of_range_handler() = 0;
    bool is_attacking() {return attacking;}
    std::shared_ptr<Ship> get_attack_target() const {return target.lock();};
private:
    int firepower;          // Firepower of the curiser
    double attack_range;  // Attack range of the cruiser
    bool attacking = false;
    std::weak_ptr<Ship> target;     // Target to attack (also controls whether we're
                                    // attacking or not
};

class Cruiser : public Warship {
public:
    // initialize, then output constructor message
    Cruiser(const std::string& name_, Point position_);

    // output destructor message
    ~Cruiser();

    void describe() const override;

    // respond to an attack
    void receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr) override;
protected:
    void target_out_of_range_handler() override;
};

class Torpedo_boat : public Warship {
public:
    // initialize
    Torpedo_boat(const std::string& name_, Point position_);

    ~Torpedo_boat();

    void describe() const override;

    void receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr) override;
protected:
    void target_out_of_range_handler() override;
};

#endif