#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include "View.h"

#include <map>

/* *** Map_view ***
 * The Map_view class encapsulates the data and functions needed to generate the map
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


class Map_view : public View {
public:
    // default constructor sets the default size, scale, and origin
    Map_view();

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

#endif