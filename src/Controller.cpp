#include "Controller.h"

#include "Model.h"
#include "Utility.h"
#include "View.h"
#include "Geometry.h"
#include "Ship_factory.h"
#include "Ship.h"

#include <iostream>
#include <string>
#include <map>

using namespace std;

Controller::Controller() {
    cout << "Controller constructed" << endl;
}

Controller::~Controller() {
    cout << "Controller destructed" << endl;
}

void Controller::run() {
    // Initialize command maps
    using ShipCmdMap_t = std::map<std::string, void (Controller::*) (Ship*)>;
    using GenericCmdMap_t = std::map<std::string, void (Controller::*) ()>;
    static ShipCmdMap_t ship_cmd_map = {
            {"course", ship_course_cmd},
            {"position", ship_position_cmd},
            {"destination", ship_dest_cmd},
            {"load_at", ship_load_cmd},
            {"unload_at", ship_unload_cmd},
            {"dock_at", ship_dock_cmd},
            {"attack", ship_attack_cmd},
            {"refuel", ship_refuel_cmd},
            {"stop", ship_stop_cmd},
            {"stop_attack", ship_stop_attack_cmd}
    };
    static GenericCmdMap_t generic_cmd_map = {
            {"default", view_default_cmd},
            {"size", view_size_cmd},
            {"zoom", view_zoom_cmd},
            {"pan", view_pan_cmd},
            {"show", view_show_cmd},

            {"status", model_status_cmd},
            {"go", model_go_cmd},
            {"create", model_create_cmd}
    };

    // TODO create and attach view

    while (true) {
        string first_word;
        cin >> first_word;
        if (first_word == "quit") {
            // TODO detach view
            cout << "Done" << endl;
            return;
        } else if (g_Model_ptr->is_ship_present(first_word)) {
            string& ship_name = first_word;
            string ship_cmd;
            cin >> ship_cmd;
            auto itt = ship_cmd_map.find(ship_cmd);
            if (itt == ship_cmd_map.end())
                throw Error("Unrecognized command!");
            Ship *ship_ptr = g_Model_ptr->get_ship_ptr(first_word);
            itt->second(ship_ptr);
        } else {
            // OTHER COMMANDS

        }
    }
}

static double get_double_from_cin() {
    double d;
    if (cin >> d)
        return d;
    else
        throw Error("Expected a double!");
}

static int get_int_from_cin() {
    int i;
    if (cin >> i)
        return i;
    else
        throw Error("Expected an integer!");
}

static Point get_point_from_cin() {
    double x = get_double_from_cin();
    double y = get_double_from_cin();
    return {x, y};
}

static double get_speed_from_cin() {
    double speed = get_double_from_cin();
    if (speed < 0.)
        throw Error("Negative speed entered!");
    return speed;
}

static Island *get_island_ptr_from_cin() {
    string island_name;
    cin >> island_name;
    return g_Model_ptr->get_island_ptr(island_name);
}

void Controller::view_default_cmd() {
    view_ptr->set_defaults();
}

void Controller::view_size_cmd() {
    int size = get_int_from_cin();
    view_ptr->set_size(size);
}

void Controller::view_zoom_cmd() {
    double scale = get_double_from_cin();
    view_ptr->set_scale(scale);
}

void Controller::view_pan_cmd() {
    double x = get_double_from_cin();
    double y = get_double_from_cin();
    view_ptr->set_origin({x, y});
}

void Controller::view_show_cmd() {
    view_ptr->draw();
}

void Controller::model_status_cmd() {
    g_Model_ptr->describe();
}

void Controller::model_go_cmd() {
    g_Model_ptr->update();
}

void Controller::model_create_cmd() {
    string ship_name, ship_type;
    cin >> ship_name;
    if (ship_name.length() < 2) // TODO magic number
        Error("Name is too short!");
    if (g_Model_ptr->is_name_in_use(ship_name))
        Error("Name is invalid!");
    cin >> ship_type;
    Point point = get_point_from_cin();

    Ship *ship = create_ship(ship_name, ship_type, point);
    g_Model_ptr->add_ship(ship);
}

void Controller::ship_course_cmd(Ship* ship) {
    double heading = get_double_from_cin();
    if (heading < 0. || heading >= 360.)
        throw Error("Invalid heading entered!");
    double speed = get_speed_from_cin();
    ship->set_course_and_speed(heading, speed);
}

void Controller::ship_position_cmd(Ship* ship) {
    Point position = get_point_from_cin();
    double speed = get_speed_from_cin();
    ship->set_destination_position_and_speed(position, speed);
}

void Controller::ship_dest_cmd(Ship* ship) {
    Island *island = get_island_ptr_from_cin();
    double speed = get_speed_from_cin();
    ship->set_destination_island_and_speed(island, speed);
}

void Controller::ship_load_cmd(Ship* ship) {
    Island *island = get_island_ptr_from_cin();
    ship->set_load_destination(island);
}

void Controller::ship_unload_cmd(Ship* ship) {
    Island *island = get_island_ptr_from_cin();
    ship->set_unload_destination(island);
}

void Controller::ship_dock_cmd(Ship* ship) {
    Island *island = get_island_ptr_from_cin();
    ship->dock(island);
}

void Controller::ship_attack_cmd(Ship* ship) {
    string ship_name;
    cin >> ship_name;
    Ship *ship_to_attack = g_Model_ptr->get_ship_ptr(ship_name);
    ship->attack(ship_to_attack);
}

void Controller::ship_refuel_cmd(Ship* ship) {
    ship->refuel();
}

void Controller::ship_stop_cmd(Ship* ship) {
    ship->stop();
}

void Controller::ship_stop_attack_cmd(Ship* ship) {
    ship->stop_attack();
}
