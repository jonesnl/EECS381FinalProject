#ifndef OCEAN_MAP_H
#define OCEAN_MAP_H

#include "Geometry.h"

#include <string>
#include <vector>

/* Ocean_map Helper Class Declaration
 *
 * An Ocean_map is used to create and draw a map view of the ocean. You set the maps
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
class Ocean_map {
public:
    // Construct an Ocean_map object
    Ocean_map(int width_, int height_, double scale_, Point origin_);

    // Returns false when the object we are trying to add to the map is not added,
    // due to it lying outside the bounds of the map. Otherwise the object's name
    // is abbreviated and added to the map at the cell corresponding to "point" and
    // the function returns true.
    bool add_to_map(const std::string& obj_name, Point point);

    // Fill the map with a given character string.
    void fill(const std::string& fill_string);

    // Disable drawing the y axis when the draw() function is called. By default
    // Ocean_map will draw y axis labels.
    void disable_y_axis_labels();

    // Draw the map.
    void draw() const;
private:
    int width, height; // Width and height of the map
    std::vector<std::vector<std::string>> map; // Cell elements are contained in the 2-D map array.
    double scale; // Scale of the map
    Point origin; // Origin of the map
    bool y_axis_labels_enabled; // Controls whether we draw y axis labels.

    // Helper function that maps a point to a specific cell in the map. Returns
    // false if the location does not correspond to a map cell.
    bool get_subscripts(int &ix, int &iy, Point location);
};

#endif