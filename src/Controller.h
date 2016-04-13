#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <map>
#include <string>
#include <memory>
#include <list>

class View;
class Map_view;
class Sailing_view;
class Bridge_view;
class Ship_component;

/* Controller
 * This class is responsible for controlling the Model and View according to
 * interactions with the user. The Controller is responsible for creating all
 * views and attaching them to the Model.
 *
 * Every command a user can enter has a corresponding private member function,
 * see the member functions below for a list of supported commands.
*/

class Controller {
public:
    // Run the program by accepting user commands, creating and controlling
    // views and ships as requested by the user
    void run();

private:
    // View containers. map_view and sailing_view are null when they are not in use,
    // and bridge_view_map contains a map of all bridge views, with the keys being
    // the corresponding ship's name.
    std::shared_ptr<Map_view> map_view;
    std::shared_ptr<Sailing_view> sailing_view;
    std::map<std::string, std::shared_ptr<Bridge_view>> bridge_view_map;

    // all_views contains a list of all views open so far, in the order in which
    // they were opened. This allows us to output each view in that order when
    // the 'show' command is entered.
    std::list<std::shared_ptr<View>> all_views;

    /******** Helper commands called in run() ********/
    // There is also a "quit" command that is handled by run(). It quits the program.

    // "open_map_view": Opens a map of the ocean, can be controlled by
    // "default", "size", "zoom", and "pan" commands. Throws an error if the
    // map is already open
    void open_map_view();

    // "open_sailing_view": Opens a view that displays some statistics of each
    // ship. Throws an error if the view is already open.
    void open_sailing_view();

    // "open_bridge_view <ship_name>": Opens a view that shows a 180 degree
    // view of what is in front of <ship_name>. Throws an error if the ship doesn't
    // exist or if that ship's bridge view is already open.
    void open_bridge_view();

    // "close_map_view": Closes the map view. Throws an error of the map view was not open.
    void close_map_view();

    // "close_sailing_view": Closes the sailing view (statistics view). Throws
    // an error if the view was not open.
    void close_sailing_view();

    // "close_bridge_view <ship_name>": Closes the bridge view of <ship_name>.
    // Throws an error if that bridge view was not open.
    void close_bridge_view();

    // "default": Resets the map_view to its default values. See the Map_view class for
    // default values.
    void map_default_cmd();

    // "size <view_size>": Sets the size of the map view to <view_size> where
    // <view_size> is the integer number of cells per row and column in the map.
    // Throws an error if the map view is not open.
    void map_size_cmd();

    // "zoom <zoom>": Sets the zoom of the map view to <zoom>. <zoom> is a double
    // measured in nm/cell. Throws an error if the map view is not open.
    void map_zoom_cmd();

    // "pan <x> <y>": Sets the origin of the map view to (<x>, <y>), where <x> and <y>
    // are doubles. Throws an error if the map view is not open.
    void map_pan_cmd();

    // "show": Shows all open views in the order in which they were opened.
    // Does not throw any errors even if there are no views open.
    void show_cmd();

    // "status": Have all islands and ships describe themselves.
    void status_cmd();

    // "go": Simulate a single tick of the simulation.
    void go_cmd();

    // "create <ship_name> <ship_type> <x> <y>": Creates a new ship with the name <ship_name>
    // and a type of <ship_type> at location (<x>, <y>). Throws an error if <ship_name>
    // is not valid, or if <ship_type> is not a valid ship type.
    void create_cmd();

    // TODO
    void create_group_cmd();

    void add_to_group_cmd();

    void remove_from_group_cmd();

    void remove_cmd();

    // "<ship_name> course <heading> <speed>": Set the ship's heading and speed.
    void ship_course_cmd(std::shared_ptr<Ship_component> ship);

    // "<ship_name> position <x> <y> <speed>": Set the ship's destination position
    // and speed.
    void ship_position_cmd(std::shared_ptr<Ship_component> ship);

    // "<ship_name> destination <island_name> <speed>": Set the ship's destination
    // island and speed.
    void ship_dest_cmd(std::shared_ptr<Ship_component> ship);

    // "<ship_name> load_at <island_name>": Tells a ship to load cargo at <island_name>.
    void ship_load_cmd(std::shared_ptr<Ship_component> ship);

    // "<ship_name> unload_at <island_name>": Tells a ship to unload cargo at <island_name>.
    void ship_unload_cmd(std::shared_ptr<Ship_component> ship);

    // "<ship_name> dock_at <island_name>": Tells a ship to attempt to dock at <island_name>.
    void ship_dock_cmd(std::shared_ptr<Ship_component> ship);

    // "<ship_name> attack <target_name>": Tells a ship to attack another ship.
    void ship_attack_cmd(std::shared_ptr<Ship_component> ship);

    // "<ship_name> refuel": Tells a ship to attempt to refuel at the docked island.
    void ship_refuel_cmd(std::shared_ptr<Ship_component> ship);

    // "<ship_name> stop": Tells a ship to stop at its current location and stop doing
    // whatever it may have been doing before.
    void ship_stop_cmd(std::shared_ptr<Ship_component> ship);

    // "<ship_name> stop_attack": Tells a ship to stop attacking.
    void ship_stop_attack_cmd(std::shared_ptr<Ship_component> ship);

    // "<ship_name> start_skimming <x> <y> <size>": Tells a ship to clean up an oil spill
    // at (<x>, <y>) with a size of <size>
    void ship_start_skimming_cmd(std::shared_ptr<Ship_component> ship);

    /******* Helper functions ********/
    // Throw an error if the map view is closed
    void if_map_view_closed_error() const;

    // Add a view to all_views, and attach the view to the model.
    void open_view_helper(std::shared_ptr<View> view);

    // Detach the view from the model and remove it from all_views.
    void close_view_helper(std::shared_ptr<View> view);
};

#endif