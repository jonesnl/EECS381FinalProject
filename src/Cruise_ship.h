#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H

/* Cruise ships are used to tour the islands on the map. When a cruise starts,
 * each island is visited exactly once starting at the destination island, visiting
 * each island based on the closes island not yet visited, then returning to where
 * the cruise started.
 *
 * Cruises start when they are told a destination island to visit. That island
 * will be the start of a cruise.
 */

#include "Ship.h"

#include <set>
#include <memory>

class Cruise_ship : public Ship {

public:
    // Construct a cruise ship
    Cruise_ship(const std::string &name_, Point position_);

    ~Cruise_ship() override;

    // Update the state of the cruise ship
    void update() override;

    // Output a description of the state of the cruise ship to cout
    void describe() const override;

    void stop() override;

    // Mark that
    virtual void set_destination_position_and_speed(Point destination_position,
            double speed) override;

    virtual void set_destination_island_and_speed(
            std::shared_ptr<Island> destination_island, double speed) override;

    virtual void set_course_and_speed(double course, double speed) override;

private:
    enum class CruiseState_t {
        not_cruising, cruising, arriving, loading_unloading,
        departing
    } cruise_state;
    std::shared_ptr<Island> origin_island;
    std::shared_ptr<Island> cruise_destination;
    std::set<std::shared_ptr<Island>> islands_to_visit;

    void new_cruise(std::shared_ptr<Island> origin_island, double speed);
    void cruise_to(std::shared_ptr<Island> destination_island, double speed);
    std::shared_ptr<Island> next_island_to_visit();
    void cancel_cruise();
    void clear_cruise_data();
};


#endif
