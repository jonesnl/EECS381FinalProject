/*
A Cruiser is a Ship with moderate speed, firepower, and resistance.
When told to attack it will start firing at the target if it is in range.
At each update, it will stop attacking if the target is either no longer afloat
(i.e. is sinking or sunk), or is out of range. As long as the target is both afloat
and in range, it will keep firing at it.
*/


#ifndef CRUISER_H
#define CRUISER_H

#include "Ship.h"

class Cruiser : public Ship {
public:
	// initialize, then output constructor message
	Cruiser(const std::string& name_, Point position_);
	// output destructor message
	~Cruiser();
	
	// perform Cruiser-specific behavior
	void update() override;

	// Cruisers will act on an attack and stop_attack command
	// will	throw Error("Cannot attack!") if not Afloat
	// will throw Error("Warship may not attack itself!") if supplied target is the same as this Warship
	void attack(Ship* target_ptr_) override;

	// will throw Error("Was not attacking!") if not Attacking
	void stop_attack() override;
	
	void describe() const override;

    void receive_hit(int hit_force, Ship* attacker_ptr) override;
	
private:
	enum class State {ATTACKING, NOT_ATTACKING};

	int firepower;				// how much force in each hit
	double maximum_range;		// maximum distance to target in order to fire
	State attack_state = State::NOT_ATTACKING;
	Ship* target_ptr = nullptr;			// pointer to target ship

	// helper functions
	// return true if this Warship is in the attacking state
	bool is_attacking() const
		{return attack_state == State::ATTACKING;}
	
	// fire at the current target
	void fire_at_target();
		
	// is the current target in range?
	bool target_in_range() const;

	// get the target
	Ship* get_target() const
		{return target_ptr;}	

};


#endif
