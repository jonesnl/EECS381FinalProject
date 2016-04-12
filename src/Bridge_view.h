#ifndef BRIDGE_VIEW_H
#define BRIDGE_VIEW_H

#include "Grid_location_view.h"

#include <map>

/* *** Bridge_view ***
 * Tracks the position and course of a ship (ownship), and the location of other
 * ships and islands in the simulation and draws a view from the bridge of the ship.
 *
 * The bridge view from the ship faces the course of the ship and it's width is
 * 190 degrees, from -90 degrees bow angle up to (but not including) 100 degrees bow angle.
 *
 * If a ship sinks, the Bridge_view will ignore new updates from the model
 * so that if another ship with the same name is constructed, it will not change
 * where the original ship sunk at.
 */

class Bridge_view : public Grid_location_view {
public:
    // Construct with ownship's name
    Bridge_view(const std::string& ship_name);

    // Track the location of ownship
    void update_location(const std::string& name, Point point) override;

    // Update the course of ownship
    void update_course(const std::string& name, double course) override;

    // Track if ownship has sunk
    void update_remove(const std::string& name) override;

    // Draw the view from ownship's bridge. If the ship is sunk, draw a special view
    void draw() const override;

protected:
    void objects_not_in_grid_handler(
            const std::vector<std::string>& objects_outside_map) const override;

    Point translate_point_handler(Point point) const override;

private:
    std::string ownship_name; // The name of the ship that we are looking from
    Point ownship_location;
    double ownship_heading; // Track the heading of ownship
    bool sunk; // Track if ownship has sunk
};

#endif