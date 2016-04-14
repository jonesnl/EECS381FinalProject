#include "Bridge_view.h"

#include "Grid_location_view.h"
#include "Navigation.h"
#include "Utility.h"

#include <iostream>

using namespace std;

const Point bridge_view_origin_c = {-90., 0.};
const double bridge_view_scale_c = 10.;
const int bridge_view_map_width_c = 19;
const int bridge_view_map_height_c = 3;
const double bridge_view_draw_dist_c = 20.;
const string bridge_view_underwater_view_c =
        "     w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-\n"
        "     w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-\n"
        "     w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-\n"
        "   -90   -60   -30     0    30    60    90\n";

// Construct a bridge view for a ship called ship_name_
Bridge_view::Bridge_view(const string& ship_name_) :
        ownship_name(ship_name_), sunk(false) {
    disable_y_axis_labels();
}

// Update the location of objects on the ocean
void Bridge_view::update_location(const string& name, Point point) {
    Grid_location_view::update_location(name, point);
    if (!sunk && name == ownship_name)
        ownship_location = point;
}

// Update the course of ownship
void Bridge_view::update_course(const string &name, double course) {
    Grid_location_view::update_course(name, course);
    if (!sunk && name == ownship_name)
        ownship_heading = course;
}

// Remove objects from the ocean if they sink. If we are removed, that means
// that ownship has sunk
void Bridge_view::update_remove(const string& name) {
    Grid_location_view::update_remove(name);
    if (name == ownship_name)
        sunk = true;
}

// Draw the view from the bridge of the ship
void Bridge_view::draw() const {
    if (sunk) {
        // If the ship is sunk, use a special bridge view that shows only water
        cout << "Bridge view from " << ownship_name <<
                " sunk at " << ownship_location << endl;

        cout << bridge_view_underwater_view_c;
    } else {
        // If we're afloat, print information about ownship
        cout << "Bridge view from " << ownship_name <<
                " position " << ownship_location <<
                " heading " << ownship_heading << endl;

        Grid_location_view::draw(bridge_view_map_width_c, bridge_view_map_height_c,
                bridge_view_scale_c, bridge_view_origin_c);
    }
}

// Translate a point on the ocean to a location on a 19x1 grid that is the view
// off the front of ownship
pair<bool, Point> Bridge_view::translate_point_handler(Point point) const {
    Compass_position position(ownship_location, point);

    // If the object is out of range, do not show it on the grid by setting the
    // first element of the returned pair to false.
    if (position.range > bridge_view_draw_dist_c ||
            position.range < double_close_enough_c)
        return {false, {0., 0.}};

    // Figure out the angle off the bow the target object is at
    double bow_angle = position.bearing - ownship_heading;
    // Make the resulting bow_angle between -180 and 180 degrees
    if (bow_angle > 180.)
        bow_angle -= 360.;
    else if (bow_angle < -180.)
        bow_angle += 360.;
    return {true, {bow_angle, 0}};
}
