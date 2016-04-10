#include "Controller.h"

#include "Model.h"
#include "Utility.h"
#include "Ship_factory.h"
#include "Ship.h"
#include "Ship_composite.h"
#include "Map_view.h"
#include "Sailing_view.h"
#include "Bridge_view.h"

#include <iostream>
#include <algorithm>

/* Controls the simulation by taking in user commands, executing member functions
 * based on the commands of the user, and letting those member functions handle
 * implimenting each command by controlling views, ships, and the simulation.
 *
 * There are two distinct types of commands, ship commands that start with a ship's
 * name, and general commands that do not. These commands are seperated into two
 * different maps in Controller::run(), and ship member functions start with "ship_".
 */

using namespace std;
using namespace placeholders;

static double get_double_from_cin();
static int get_int_from_cin();
static Point get_point_from_cin();
static double get_speed_from_cin();
static shared_ptr<Island> get_island_ptr_from_cin();


/* Public Function Definitions */

// Take user input in a loop and call the corresponding member function to handle that
// input.
void Controller::run() {
    // Initialize command maps, used to map user input to the corresponding member
    // functions that handle that input.

    // Command map for commands that start with a ship's name.
    map<string, void (Controller::*) (shared_ptr<Ship_component>)> ship_cmd_map = {
            {"course", &Controller::ship_course_cmd},
            {"position", &Controller::ship_position_cmd},
            {"destination", &Controller::ship_dest_cmd},
            {"load_at", &Controller::ship_load_cmd},
            {"unload_at", &Controller::ship_unload_cmd},
            {"dock_at", &Controller::ship_dock_cmd},
            {"attack", &Controller::ship_attack_cmd},
            {"refuel", &Controller::ship_refuel_cmd},
            {"stop", &Controller::ship_stop_cmd},
            {"stop_attack", &Controller::ship_stop_attack_cmd},
            {"start_skimming", &Controller::ship_start_skimming_cmd}
    };

    // Command map for commands that do not start with a ship's name.
    map<string, void (Controller::*) ()> generic_cmd_map = {
            {"open_map_view", &Controller::open_map_view},
            {"open_sailing_view", &Controller::open_sailing_view},
            {"open_bridge_view", &Controller::open_bridge_view},
            {"close_map_view", &Controller::close_map_view},
            {"close_sailing_view", &Controller::close_sailing_view},
            {"close_bridge_view", &Controller::close_bridge_view},
            {"default", &Controller::map_default_cmd},
            {"size", &Controller::map_size_cmd},
            {"zoom", &Controller::map_zoom_cmd},
            {"pan", &Controller::map_pan_cmd},
            {"show", &Controller::show_cmd},

            {"status", &Controller::status_cmd},
            {"go", &Controller::go_cmd},
            {"create", &Controller::create_cmd},

            {"create_group", &Controller::create_group_cmd},
            {"add_to_group", &Controller::add_to_group_cmd},
            {"remove_from_group", &Controller::remove_from_group_cmd},
            {"remove_group", &Controller::remove_group_cmd}
    };

    // Continue to take in user's input
    while (true) {
        try {
            cout << "\nTime " << Model::get_inst()->get_time() << ": Enter command: ";
            string command;
            // Get the command name
            cin >> command;
            if (command == "quit") { // Handle the "quit" command separately.
                cout << "Done" << endl;
                return;
            } else if (Model::get_inst()->is_ship_component_present(command)) {
                // If command is a ship's name, then this is a ship command

                string &component_name = command; // Alias command to a more meaningful name

                string ship_cmd;
                cin >> ship_cmd; // Get the name of the ship command

                auto itt = ship_cmd_map.find(ship_cmd);
                if (itt == ship_cmd_map.end())
                    throw Error("Unrecognized command!");

                // Get the ship's pointer from the model.
                auto ship_component_ptr =
                        Model::get_inst()->get_ship_component_ptr(component_name);

                // Call the handling ship command function.
                (this->*itt->second)(ship_component_ptr);
            } else {
                // If it is not a ship command, it must be a regular command
                auto itt = generic_cmd_map.find(command);
                if (itt == generic_cmd_map.end())
                    throw Error("Unrecognized command!");

                // Call the handling command function.
                (this->*itt->second)();
            }
        } catch (Error& e) {
            // Handle errors thrown by our program by printing the error message,
            // clearing the input error flags if set, and ignoring the rest of the
            // input line. Then continue the program.
            cout << e.what() << endl;
            cin.clear();
            while (cin.peek() != '\n')
                cin.get();
        } catch (std::exception& e) {
            // Handle errors thrown by the standard library by printing an error
            // message and exiting the program.
            cout << e.what() << endl;
            cout << "Done" << endl;
            return;
        }
    }
}

/* Private Helper Function Definitions */

// Create and open the map view
void Controller::open_map_view() {
    if (map_view)
        throw Error("Map view is already open!");
    map_view = make_shared<Map_view>();
    open_view_helper(map_view);
}

// Create and open the sailing view
void Controller::open_sailing_view() {
    if (sailing_view)
        throw Error("Sailing data view is already open!");
    sailing_view = make_shared<Sailing_view>();
    open_view_helper(sailing_view);
}

// Create and open a bridge view for a given ship. Throw an error if the ship
// doesn't exist or if the view is already open.
void Controller::open_bridge_view() {
    string ship_name;
    cin >> ship_name;
    if (!Model::get_inst()->is_ship_present(ship_name))
        throw Error("Ship not found!");
    auto bridge_view_ptr = make_shared<Bridge_view>(ship_name);
    bool success = bridge_view_map.emplace(ship_name, bridge_view_ptr).second;
    if (!success)
        throw Error("Bridge view is already open for that ship!");
    open_view_helper(bridge_view_ptr);
}

// Close the map view.
void Controller::close_map_view() {
    if_map_view_closed_error();
    close_view_helper(map_view);
    map_view = nullptr;
}

// Close the sailing view.
void Controller::close_sailing_view() {
    if (!sailing_view)
        throw Error("Sailing data view is not open!");
    close_view_helper(sailing_view);
    sailing_view = nullptr;
}

// Close the bridge view for a given ship.
void Controller::close_bridge_view() {
    string ship_name;
    cin >> ship_name;
    auto bridge_view_itt = bridge_view_map.find(ship_name);
    if (bridge_view_itt == bridge_view_map.end())
        throw Error("Bridge view for that ship is not open!");
    auto bridge_view_ptr = bridge_view_itt->second;
    bridge_view_map.erase(bridge_view_itt);
    close_view_helper(bridge_view_ptr);
}

// Set the defaults for the map view
void Controller::map_default_cmd() {
    if_map_view_closed_error();
    map_view->set_defaults();
}

// Set the map view's size
void Controller::map_size_cmd() {
    if_map_view_closed_error();
    int size = get_int_from_cin();
    map_view->set_size(size);
}

// Set the map view's zoom
void Controller::map_zoom_cmd() {
    if_map_view_closed_error();
    double scale = get_double_from_cin();
    map_view->set_scale(scale);
}

// Set the map view's origin
void Controller::map_pan_cmd() {
    if_map_view_closed_error();
    double x = get_double_from_cin();
    double y = get_double_from_cin();
    map_view->set_origin({x, y});
}

// Show all views in the order in which they were opened.
void Controller::show_cmd() {
    for_each(all_views.begin(), all_views.end(),
            mem_fn(&View::draw));
}

// Show the status of all objects.
void Controller::status_cmd() {
    Model::get_inst()->describe();
}

// Simulate a single timestep in the simulation.
void Controller::go_cmd() {
    Model::get_inst()->update();
}

// Create a new ship.
void Controller::create_cmd() {
    string ship_name;
    cin >> ship_name;

    // If the name is too short for the views to abbreviate correctly
    if (ship_name.length() < name_abbreviation_length_c)
        throw Error("Name is too short!");

    // If the name is already in use by an island or ship name. Also compares
    // if name abbreviations would conflict.
    if (Model::get_inst()->is_name_in_use(ship_name))
        throw Error("Name is invalid!");

    string ship_type;
    cin >> ship_type;

    Point point = get_point_from_cin();

    shared_ptr<Ship> ship = create_ship(ship_name, ship_type, point);
    Model::get_inst()->add_ship(ship);
}

// TODO
void Controller::create_group_cmd() {
    string group_name;
    cin >> group_name;
    if(Model::get_inst()->is_name_in_use(group_name))
        throw Error("Name is invalid!");

    auto group_ptr = make_shared<Ship_composite>(group_name);
    Model::get_inst()->add_group(group_ptr);
}

void Controller::add_to_group_cmd() {
    string group_name;
    cin >> group_name;
    auto group_ptr = Model::get_inst()->get_ship_composite_ptr(group_name);
    string component_name;
    cin >> component_name;
    auto component_ptr = Model::get_inst()->get_ship_component_ptr(component_name);
    group_ptr->add_component(component_ptr);
}

void Controller::remove_from_group_cmd() {
    string group_name;
    cin >> group_name;
    auto group_ptr = Model::get_inst()->get_ship_composite_ptr(group_name);
    string component_name;
    cin >> component_name;
    group_ptr->remove_component(component_name);
}

void Controller::remove_group_cmd() {
    string group_name;
    cin >> group_name;
    auto group_ptr = Model::get_inst()->get_ship_composite_ptr(group_name);
    Model::get_inst()->remove_group(group_ptr);
}

// Set the course and speed of a ship.
void Controller::ship_course_cmd(shared_ptr<Ship_component> ship) {
    double heading = get_double_from_cin();

    // If the entered heading is not valid heading according to
    // nautical navigation conventions.
    if (heading < 0. || heading >= 360.)
        throw Error("Invalid heading entered!");

    double speed = get_speed_from_cin();
    ship->set_course_and_speed(heading, speed);
}

// Set the destination position and speed of a ship.
void Controller::ship_position_cmd(shared_ptr<Ship_component> ship) {
    Point position = get_point_from_cin();
    double speed = get_speed_from_cin();
    ship->set_destination_position_and_speed(position, speed);
}

// Set the destination island and speed of a ship.
void Controller::ship_dest_cmd(shared_ptr<Ship_component> ship) {
    shared_ptr<Island> island = get_island_ptr_from_cin();
    double speed = get_speed_from_cin();
    ship->set_destination_island_and_speed(island, speed);
}

// Set the loading location of a ship.
void Controller::ship_load_cmd(shared_ptr<Ship_component> ship) {
    shared_ptr<Island> island = get_island_ptr_from_cin();
    ship->set_load_destination(island);
}

// Set the unloading location of a ship.
void Controller::ship_unload_cmd(shared_ptr<Ship_component> ship) {
    shared_ptr<Island> island = get_island_ptr_from_cin();
    ship->set_unload_destination(island);
}

// Tell the ship to dock at an island.
void Controller::ship_dock_cmd(shared_ptr<Ship_component> ship) {
    shared_ptr<Island> island = get_island_ptr_from_cin();
    ship->dock(island);
}

// Tell the ship to attack another ship
void Controller::ship_attack_cmd(shared_ptr<Ship_component> ship) {
    string target_name;
    cin >> target_name;
    shared_ptr<Ship> target = Model::get_inst()->get_ship_ptr(
            target_name);
    ship->attack(target);
}

// Tell the ship to refuel at the docked island.
void Controller::ship_refuel_cmd(shared_ptr<Ship_component> ship) {
    ship->refuel();
}

// Tell the ship to stop what it is doing.
void Controller::ship_stop_cmd(shared_ptr<Ship_component> ship) {
    ship->stop();
}

// Tell the ship to stop attacking.
void Controller::ship_stop_attack_cmd(shared_ptr<Ship_component> ship) {
    ship->stop_attack();
}

// Tell ship to skim an oil spill
void Controller::ship_start_skimming_cmd(std::shared_ptr<Ship_component> ship) {
    Point spill_loc = get_point_from_cin();
    int size = get_int_from_cin();
    ship->start_skimming(spill_loc, size);
}

/******* Helper member functions *********/
// Throw an error if the map view is not open.
void Controller::if_map_view_closed_error() const {
    if (!map_view)
        throw Error("Map view is not open!");
}

// Add a view to all_views and attach it to the model.
void Controller::open_view_helper(std::shared_ptr<View> view) {
    all_views.push_back(view);
    Model::get_inst()->attach(view);
}

// Detach a view from the model, and erase it from all_views.
void Controller::close_view_helper(std::shared_ptr<View> view) {
    Model::get_inst()->detach(view);
    all_views.erase(find(all_views.begin(), all_views.end(), view));
}

/* General helper functions */
// Get a double from the user, throw an error if the input failed.
static double get_double_from_cin() {
    double d;
    if (cin >> d)
        return d;
    else
        throw Error("Expected a double!");
}

// Get an integer from the user, throw an error if the input failed.
static int get_int_from_cin() {
    int i;
    if (cin >> i)
        return i;
    else
        throw Error("Expected an integer!");
}

// Get a point from the user, throws an error if either inputs fail.
static Point get_point_from_cin() {
    double x = get_double_from_cin();
    double y = get_double_from_cin();
    return {x, y};
}

// Gets a speed from cin. Throws an error if the speed is negative.
static double get_speed_from_cin() {
    double speed = get_double_from_cin();
    if (speed < 0.)
        throw Error("Negative speed entered!");
    return speed;
}

// Gets an island pointer from the model based on the island's name the user enters.
static shared_ptr<Island> get_island_ptr_from_cin() {
    string island_name;
    cin >> island_name;
    return Model::get_inst()->get_island_ptr(island_name);
}
