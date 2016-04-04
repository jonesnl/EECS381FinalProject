#ifndef SAILING_VIEW_H
#define SAILING_VIEW_H

#include "View.h"

#include <map>

/* Stores the course, speed, and fuel information of the ships of the simulation.
 *
 * The draw() function prints out that stored informtation to the user to use.
 *
 * Listens to the course, speed, and fuel information feeds.
 */

class Sailing_view : public View {
public:
    // Store the course of a ship
    void update_course(const std::string& name, double course) override;

    // Store the speed of a ship
    void update_speed(const std::string& name, double speed) override;

    // Store the fuel information of a ship
    void update_fuel(const std::string& name, double fuel) override;

    // Remove the information of a ship
    void update_remove(const std::string& name) override;

    // Print out the course, speed, and fuel information of all ships to the user
    void draw() const override;

private:
    // Structure to store the course, speed, and fuel of each ship.
    struct SailingData {
        double course;
        double speed;
        double fuel;
    };

    // Map of ship names to their sailing data
    std::map<std::string, SailingData> sailing_data_map;
};

#endif