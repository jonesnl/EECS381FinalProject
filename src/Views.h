#ifndef VIEWS_H
#define VIEWS_H

#include "View.h"

#include <string>
#include <map>

/* *** MapView class ***
 * The MapView class encapsulates the data and functions needed to generate the map
 * display, and control its properties. It has a "memory" for the names and locations
 * of the to-be-plotted objects.
 *
 * As needed, change the origin, scale, or displayed size of the map
 * with the appropriate functions. Since the view "remembers" the previously updated
 * object data, the draw function will print out a map showing the previous objects
 * using the new settings.
 *
 * Listens to the location information feed.
 */


class MapView : public View {
public:
    // default constructor sets the default size, scale, and origin
    MapView();

    // Save the supplied object's location for future use in a draw() call
    // If the name is already present,the new location replaces the previous one.
    void update_location(const std::string& name, Point location) override;

    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;

    // prints out the current map
    void draw() const override;

    // modify the display parameters
    // if the size is out of bounds will throw Error("New map size is too big!")
    // or Error("New map size is too small!")
    void set_size(int size_);

    // If scale is not positive, will throw Error("New map scale must be positive!");
    void set_scale(double scale_);

    // any values are legal for the origin
    void set_origin(Point origin_);

    // set the parameters to the default values
    void set_defaults();

private:
    std::map<std::string, Point> location_map;	// Map of the locations of ships and islands
    int size; // Size of the map in number of rows and columns
    double scale; // Scale of the map in nm/grid_item
    Point origin; // Location of the origin of the view.
};

/* *** SailingView ***
 * Stores the course, speed, and fuel information of the ships of the simulation.
 *
 * The draw() function prints out that stored informtation to the user to use.
 *
 * Listens to the course, speed, and fuel information feeds.
 */

class SailingView : public View {
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

/* *** BridgeView *** TODO expand on this comment
 * Tracks the position and course of a ship (ownship), and the location of other
 * ships and islands in the simulation and draws a view from the bridge of the ship.
 *
 * The bridge view from the ship faces the course of the ship and it's width is
 * 190 degrees, from -90 degrees bow angle up to (but not including) 100 degrees bow angle.
 */

class BridgeView : public View {
public:
    // Construct with ownship's name
    BridgeView(const std::string& ship_name);

    // Update the location of either ownship or another object
    void update_location(const std::string& name, Point point) override;

    // Update the course of ownship
    void update_course(const std::string& name, double course) override;

    // Remove another object from the location table, or if it's the same name
    // as ownship, note that the ship has sunk
    void update_remove(const std::string& name) override;

    // Draw the view from ownship's bridge. If the ship is sunk, draw a special view
    void draw() const override;

private:
    std::string ownship; // The name of the ship that we are looking from
    std::map<std::string, Point> location_map; // Location of other objects on the ocean
    bool sunk = false; // Track if ownship has sunk
    double ship_heading; // Track the heading of ownship
    Point ship_location; // Track the location of ownship
};

#endif
