#include "Controller.h"

#include "Model.h"
#include "Utility.h"
#include "View.h"
#include "Ship_factory.h"
#include "Ship.h"

#include <iostream>
#include <memory>

using namespace std;

Controller::Controller() {
    cout << "Controller constructed" << endl;
}

Controller::~Controller() {
    cout << "Controller destructed" << endl;
}

void Controller::run() {
    // Initialize command maps
    using ShipCmdMap_t = std::map<std::string, void (Controller::*) (shared_ptr<Ship>)>;
    using GenericCmdMap_t = std::map<std::string, void (Controller::*) ()>;
    static ShipCmdMap_t ship_cmd_map = {
            {"course", &Controller::ship_course_cmd},
            {"position", &Controller::ship_position_cmd},
            {"destination", &Controller::ship_dest_cmd},
            {"load_at", &Controller::ship_load_cmd},
            {"unload_at", &Controller::ship_unload_cmd},
            {"dock_at", &Controller::ship_dock_cmd},
            {"attack", &Controller::ship_attack_cmd},
            {"refuel", &Controller::ship_refuel_cmd},
            {"stop", &Controller::ship_stop_cmd},
            {"stop_attack", &Controller::ship_stop_attack_cmd}
    };
    static GenericCmdMap_t generic_cmd_map = {
            {"default", &Controller::view_default_cmd},
            {"size", &Controller::view_size_cmd},
            {"zoom", &Controller::view_zoom_cmd},
            {"pan", &Controller::view_pan_cmd},
            {"show", &Controller::view_show_cmd},

            {"status", &Controller::model_status_cmd},
            {"go", &Controller::model_go_cmd},
            {"create", &Controller::model_create_cmd}
    };

    view_ptr = make_shared<View>();
    g_Model_ptr->attach(view_ptr);

    while (true) {
        try {
            cout << "\nTime " << g_Model_ptr->get_time() << ": Enter command: ";
            string first_word;
            cin >> first_word;
            if (first_word == "quit") {
                quit_helper();
                return;
            } else if (g_Model_ptr->is_ship_present(first_word)) {
                string &ship_name = first_word;
                string ship_cmd;
                cin >> ship_cmd;
                auto itt = ship_cmd_map.find(ship_cmd);
                if (itt == ship_cmd_map.end())
                    throw Error("Unrecognized command!");
                shared_ptr<Ship> ship_ptr = g_Model_ptr->get_ship_ptr(ship_name);
                (this->*itt->second)(ship_ptr); // TODO reformat?
            } else {
                auto itt = generic_cmd_map.find(first_word);
                if (itt == generic_cmd_map.end())
                    throw Error("Unrecognized command!");
                (this->*itt->second)();
            }
        } catch (Error& e) {
            cout << e.what() << endl;
            cin.clear();
            while (cin.peek() != '\n')
                cin.get();
        } catch (std::exception& e) {
            cout << e.what() << endl;
            quit_helper();
            return;
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

static shared_ptr<Island> get_island_ptr_from_cin() {
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
        throw Error("Name is too short!");
    if (g_Model_ptr->is_name_in_use(ship_name))
        throw Error("Name is invalid!");
    cin >> ship_type;
    Point point = get_point_from_cin();

    shared_ptr<Ship> ship = create_ship(ship_name, ship_type, point);
    g_Model_ptr->add_ship(ship);
}

void Controller::ship_course_cmd(shared_ptr<Ship> ship) {
    double heading = get_double_from_cin();
    if (heading < 0. || heading >= 360.)
        throw Error("Invalid heading entered!");
    double speed = get_speed_from_cin();
    ship->set_course_and_speed(heading, speed);
}

void Controller::ship_position_cmd(shared_ptr<Ship> ship) {
    Point position = get_point_from_cin();
    double speed = get_speed_from_cin();
    ship->set_destination_position_and_speed(position, speed);
}

void Controller::ship_dest_cmd(shared_ptr<Ship> ship) {
    shared_ptr<Island> island = get_island_ptr_from_cin();
    double speed = get_speed_from_cin();
    ship->set_destination_island_and_speed(island, speed);
}

void Controller::ship_load_cmd(shared_ptr<Ship> ship) {
    shared_ptr<Island> island = get_island_ptr_from_cin();
    ship->set_load_destination(island);
}

void Controller::ship_unload_cmd(shared_ptr<Ship> ship) {
    shared_ptr<Island> island = get_island_ptr_from_cin();
    ship->set_unload_destination(island);
}

void Controller::ship_dock_cmd(shared_ptr<Ship> ship) {
    shared_ptr<Island> island = get_island_ptr_from_cin();
    ship->dock(island);
}

void Controller::ship_attack_cmd(shared_ptr<Ship> ship) {
    string ship_name;
    cin >> ship_name;
    shared_ptr<Ship> ship_to_attack = g_Model_ptr->get_ship_ptr(ship_name);
    ship->attack(ship_to_attack);
}

void Controller::ship_refuel_cmd(shared_ptr<Ship> ship) {
    ship->refuel();
}

void Controller::ship_stop_cmd(shared_ptr<Ship> ship) {
    ship->stop();
}

void Controller::ship_stop_attack_cmd(shared_ptr<Ship> ship) {
    ship->stop_attack();
}

void Controller::quit_helper() {
    g_Model_ptr->detach(view_ptr);
    cout << "Done" << endl;
}