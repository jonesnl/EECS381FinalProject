#ifndef VIEWS_H
#define VIEWS_H
/* *** View class ***
The View class encapsulates the data and functions needed to generate the map
display, and control its properties. It has a "memory" for the names and locations
of the to-be-plotted objects.

Usage:
1. Call the update_location function with the name and position of each object
to be plotted. If the object is not already in the View's memory, it will be added
along with its location. If it is already present, its location will be set to the
supplied location. If a single object changes location, its location can be separately
updated with a call to update_location.

2. Call the update_remove function with the name of any object that should
no longer be plotted. This must be done *after* any call to update_location that
has the same object name since update_location will add any object name supplied.

3. Call the draw function to print out the map.

4. As needed, change the origin, scale, or displayed size of the map
with the appropriate functions. Since the view "remembers" the previously updated
information, the draw function will print out a map showing the previous objects
using the new settings.
*/

#include "View.h"

#include <string>
#include <map>

class MapView : public View {
public:
    // default constructor sets the default size, scale, and origin, outputs constructor message
    MapView();

    // Save the supplied name and location for future use in a draw() call
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

    // If scale is not postive, will throw Error("New map scale must be positive!");
    void set_scale(double scale_);

    // any values are legal for the origin
    void set_origin(Point origin_);

    // set the parameters to the default values
    void set_defaults();

private:
    std::map<std::string, Point> location_map;	// Locations of ships and islands
    int size;
    double scale;
    Point origin;
};

class SailingView : public View {
public:
    void update_course(const std::string& name, double course) override;

    void update_speed(const std::string& name, double speed) override;

    void update_fuel(const std::string& name, double fuel) override;

    void update_remove(const std::string& name) override;

    void draw() const override;

private:
    struct SailingData { // TODO remove setinal
        double course = -300.;
        double speed = -300.;
        double fuel = -300.;
    };
    std::map<std::string, SailingData> sailing_data_map;
};

class BridgeView : public View {
public:
    BridgeView(const std::string& ship_name);
    void update_location(const std::string& name, Point point) override;

    void update_course(const std::string& name, double course) override;

    void update_remove(const std::string& name) override;

    void draw() const override;

private:
    // TODO think about static location map
    std::map<std::string, Point> location_map;
    std::string ship_name;
    bool sunk = false;
    double ship_heading;
    Point ship_location;
};

#endif
