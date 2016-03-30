#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H

/* Cruise ships are used to tour the islands on the map. When a cruise starts,
 * each island is visited exactly once starting at the destination island, visiting
 * each island based on the closes island not yet visited, then returning to where
 * the cruise started.
 *
 * Cruises start when they are told a destination island to visit. That island
 * will be the start of a cruise. A cruise is cancelled if another destination
 * or course is set during a cruise, or if the ship is told to stop.
 *
 * The speed of the cruise is the speed set by the set_destination_and_speed() function.
 */

#include "Ship.h"
#include "Utility.h"

#include <memory>
#include <string>
#include <set>

class Cruise_ship : public Ship {
public:
    // Construct a cruise ship
    Cruise_ship(const std::string &name_, Point position_);

    // Update the state of the cruise ship
    void update() override;

    // Output a description of the state of the cruise ship to cout
    void describe() const override;

    // Cancel currnet cruise and stop the ship
    void stop() override;

    // Cancel current cruise and start moving towards destination_position
    virtual void set_destination_position_and_speed(Point destination_position,
            double speed) override;

    // Cancel current cruise and start a new cruise at destination_island
    virtual void set_destination_island_and_speed(
            std::shared_ptr<Island> destination_island, double speed) override;

    // Cancel current cruise and start moving on the provided course
    virtual void set_course_and_speed(double course, double speed) override;

private:
    // Track the state of the cruise ship. arriving, loading_unloading, departing
    // are all states used when we are docked at an island during a cruise
    enum class CruiseState_t {
        not_cruising, cruising, arriving, loading_unloading, departing
    } cruise_state;

    // Where we started a cruise at, AKA where we need to return to at the end
    // of the cruise
    std::shared_ptr<Island> origin_island;

    // Track the speed of the cruise
    double cruise_speed;

    // Track the current destination of the cruise
    std::shared_ptr<Island> next_destination;

    // Track the set of islands that we still need to visit on the cruise
    std::set<std::shared_ptr<Island>, IslandNameComp> islands_to_visit;

    // Start cruising to destination_island
    void cruise_to(std::shared_ptr<Island> destination_island);

    // Figure out what is the next island we should visit on the cruise
    std::shared_ptr<Island> next_island_to_visit();

    // Cancel the current cruise
    void cancel_cruise();

    // Clear data associated with a cruise
    void clear_cruise_data();
};


#endif
