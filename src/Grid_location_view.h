#ifndef OCEAN_MAP_H
#define OCEAN_MAP_H

#include "Geometry.h"
#include "View.h"

#include <string>
#include <vector>
#include <map>

/* Grid_location_view Intermediate Class Declaration
 *
 * An Grid_location_view is used to create and draw a map view of the ocean. You set the maps
 * width, height, scale, and origin as constructor elements, then you add locations
 * to the map. If a cell in the map is filled by more than one object, it will
 * use "* " as the cell. If no object is in the map, it will use ". " for the cell.
 * If only one object is in a cell, it will use an abbreviation of the string as
 * the cell contents "Aj".
 *
 * You can fill the map with a specific string if you wish using the fill() function.
 *
 * Draw the map using the draw function.
 */
class Grid_location_view : public View {
public:
    // Construct an Grid_location_view object
    Grid_location_view();

    // Disable drawing the y axis when the draw() function is called. By default
    // we will draw y axis labels.
    void disable_y_axis_labels();

    // Track the location of an object
    void update_location(const std::string& name, Point location) override;

    // Remove the location of an object from the view
    void update_remove(const std::string& name) override;

protected:
    // Derived classes can decide to do something with a list of objects that
    // are being tracked but are not located on the grid. This is called before
    // the grid is printed to the screen during the draw(...) function.
    virtual void objects_not_in_grid_handler(
            const std::vector<std::string>& objects_outside_map) const = 0;

    // Derived classes can translate a tracked location as provided by the Model
    // to a new location if they wish. If they wish to use the same location as
    // provided by the Model, return the point argument.
    virtual Point translate_point_handler(Point point) const = 0;

    // Draw the grid, will call objects_not_in_grid_handler(...)
    // before drawing the grid to the screen.
    void draw(int width, int height, double scale, Point origin) const;

private:
    std::map<std::string, Point> location_map;	// Map of the locations of ships and islands
    bool y_axis_labels_enabled; // Controls whether we draw y axis labels.

    // Helper function that maps a point to a specific cell in the map. Returns
    // false if the location does not correspond to a map cell.
    bool get_subscripts(int &ix, int &iy, Point location,
            int width, int height, double scale, Point origin) const;
};

#endif