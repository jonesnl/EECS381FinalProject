#ifndef PIRATE_SHIP_H
#define PIRATE_SHIP_H

#include "Ship.h"

/* Skimmer class
 * A skimmer is meant to simulate the job of an oil skimmer that cleans up oil
 * after an oil spill.
 *
 * To control the skimmer, tell the skimmer to start skimming by using the start_skimming
 * function. The ship will then travel to the bottom left corner of the spill
 * (aka the spill origin), then circle around the spill, slowly spiraling inwards
 * towards the center of the spill. The skimmer has akimming equipment that reaches
 * 1 nm off the starboard side of the vessel, so paths spiral inwards by 1 nm every
 * revolution. When the skimmer has finished skimming the entire area of the spill,
 * it stops.
 *
 * If the skimmer is told to stop or is given another order, it stops skimming and
 * will follow the new order immediately.
 */

class Skimmer : public Ship {
public:
    // Construct a skimmer object
    Skimmer(const std::string& name_, Point position_);

    // Describe the skimmer
    void describe() const override;

    // Update the skimmer, and simulate the circular path if we are
    // currently skimming
    void update() override;

    // Stop skimming if we are skimming and set a new destination and speed
    void set_destination_position_and_speed(Point destination_position,
            double speed) override;

    // Stop skimming if we are skimming and set a new destination and speed.
    void set_destination_island_and_speed(std::shared_ptr<Island> destination_island,
            double speed) override;

    // Stop skimming if we are skimming and set a new course and speed
    void set_course_and_speed(double course, double speed) override;

    // Start skimming at the south-west corner of a spill of size spill_size_.
    void start_skimming(Point spill_sw_corner_, int spill_size_) override;

    // Stop skimming if we are skimming
    void stop() override;

private:
    enum class SkimmingState_t {not_skimming, going_to_spill, going_north,
        going_east, going_south, going_west} skimming_state;
    int spill_size;
    Point spill_sw_corner;
    int additional_sides_to_skim;

    // See if we are currently skimming or not
    bool is_skimming() const
        {return skimming_state != SkimmingState_t::not_skimming;};

    // Skim the first side of the spill since it needs to be specially handled
    void skim_first_side();

    // Reset the state of the skimming object
    void reset_skimming_state();
};


#endif
