#include "Bridge_view.h"

#include "Ocean_map.h"
#include "Navigation.h"
#include "Utility.h"

#include <iostream>

using namespace std;

const Point bridge_view_origin_c = {-90., 0.};
const double bridge_view_scale_c = 10.;
const int bridge_view_map_width_c = 19;
const int bridge_view_map_height_c = 3;
const double bridge_view_draw_dist_c = 20.;

// Construct a bridge view for a ship called ship_name_
Bridge_view::Bridge_view(const string& ship_name_) :
        ownship_name(ship_name_), sunk(false) { }

// Update the location of objects on the ocean
void Bridge_view::update_location(const string& name, Point point) {
    if (sunk)
        return;
    if (ownship_name == name)
        ownship_location = point;
    else
        location_map[name] = point;
}

// Update the course of ownship
void Bridge_view::update_course(const string &name, double course) {
    if (sunk)
        return;
    if (name == ownship_name)
        ownship_heading = course;
}

// Remove objects from the ocean if they sink. If we are removed, that means
// that ownship has sunk
void Bridge_view::update_remove(const string& name) {
    if (sunk)
        return;
    if (name == ownship_name)
        sunk = true;
    else
        location_map.erase(name);
}

// Draw the view from the bridge of the ship
void Bridge_view::draw() const {
    Ocean_map ocean_map(bridge_view_map_width_c, bridge_view_map_height_c,
            bridge_view_scale_c, bridge_view_origin_c);
    if (sunk) {
        // If the ship is sunk, use a special bridge view that shows only water
        cout << "Bridge view from " << ownship_name <<
        " sunk at " << ownship_location << endl;
        ocean_map.fill("w-");
    } else {
        // If we're afloat, print information about ownship
        cout << "Bridge view from " << ownship_name <<
        " position " << ownship_location <<
        " heading " << ownship_heading << endl;

        // Construct the view_array which will show what is in any given 10 degree
        // chunk of our view off the front of the bridge of our ship
        for (const auto& name_loc_pair : location_map) {
            Compass_position position(ownship_location, name_loc_pair.second);
            // If the object is out of range, do not show it to the user
            if (position.range > bridge_view_draw_dist_c ||
                position.range < double_close_enough_c)
                continue;

            // Figure out the angle off the bow the target object is at
            double bow_angle = position.bearing - ownship_heading;
            // Make the resulting bow_angle between -180 and 180 degrees
            if (bow_angle > 180.)
                bow_angle -= 360.;
            else if (bow_angle < -180.)
                bow_angle += 360.;

            // If the resulting bow_angle is not within -90 to 100 degrees, do not
            // add it to the view. ix tells us what cell of the view we should put
            // the object in.
            ocean_map.add_to_map(name_loc_pair.first, {bow_angle, 0});
        }
    }
    ocean_map.disable_y_axis_labels();
    ocean_map.draw();
}
