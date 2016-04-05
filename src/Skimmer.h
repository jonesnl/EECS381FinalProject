#ifndef PIRATE_SHIP_H
#define PIRATE_SHIP_H

#include "Ship.h"

class Skimmer : public Ship {
public:
    Skimmer(const std::string& name_, Point position_);

    void describe() const override;

    void update() override;

    void start_skimming(Point spill_origin_, double spill_size_) override;

    void stop() override;

private:
    enum class SkimmingState_t {not_skimming, going_to_spill, going_up,
        going_right, going_down, going_left} skimming_state;
    double skimmed_size;
    double spill_size;
    Point spill_origin;

    bool is_skimming() const
        {return skimming_state != SkimmingState_t::not_skimming;};
    void stop_skimming();
};


#endif
