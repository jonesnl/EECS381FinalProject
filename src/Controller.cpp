#include "Controller.h"

#include "Model.h"
#include "Utility.h"
#include "Views.h"
#include "Ship_factory.h"
#include "Ship.h"

#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;
using namespace placeholders;

Controller::Controller() {
    cout << "Controller constructed" << endl;
}

Controller::~Controller() {
    cout << "Controller destructed" << endl;
}

void Controller::run() {
    // Initialize command maps
    using ShipCmdMap_t = map<string, void (Controller::*) (shared_ptr<Ship>)>;
    using GenericCmdMap_t = map<string, void (Controller::*) ()>;
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
            {"open_map_view", &Controller::open_map_view},
            {"open_sailing_view", &Controller::open_sailing_view},
            {"open_bridge_view", &Controller::open_bridge_view},
            {"close_map_view", &Controller::close_map_view},
            {"close_sailing_view", &Controller::close_sailing_view},
            {"close_bridge_view", &Controller::close_bridge_view},
            {"default", &Controller::view_default_cmd},
            {"size", &Controller::view_size_cmd},
            {"zoom", &Controller::view_zoom_cmd},
            {"pan", &Controller::view_pan_cmd},
            {"show", &Controller::view_show_cmd},

            {"status", &Controller::model_status_cmd},
            {"go", &Controller::model_go_cmd},
            {"create", &Controller::model_create_cmd}
    };

    Model *model_ptr = Model::get_Instance();

    while (true) {
        try {
            cout << "\nTime " << model_ptr->get_time() << ": Enter command: ";
            string first_word;
            cin >> first_word;
            if (first_word == "quit") {
                quit_helper();
                return;
            } else if (model_ptr->is_ship_present(first_word)) {
                string &ship_name = first_word;
                string ship_cmd;
                cin >> ship_cmd;
                auto itt = ship_cmd_map.find(ship_cmd);
                if (itt == ship_cmd_map.end())
                    throw Error("Unrecognized command!");
                shared_ptr<Ship> ship_ptr = model_ptr->get_ship_ptr(ship_name);
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
    return Model::get_Instance()->get_island_ptr(island_name);
}

void Controller::open_map_view() {
    if (map_view)
        throw Error("Map view is already open!");
    map_view = make_shared<MapView>();
    open_view_helper(map_view);
}

void Controller::open_sailing_view() {
    if (sailing_view)
        throw Error("Sailing data view is already open!");
    sailing_view = make_shared<SailingView>();
    open_view_helper(sailing_view);
}

void Controller::open_bridge_view() {
    string ship_name;
    cin >> ship_name;
    auto bridge_view_ptr = make_shared<BridgeView>(ship_name);
    bool success = bridge_view_map.emplace(ship_name, bridge_view_ptr).second;
    if (!success)
        throw Error("Bridge view is already open for that ship!");
    open_view_helper(bridge_view_ptr);
}

void Controller::close_map_view() {
    if_map_view_closed_error();
    map_view = nullptr;
    close_view_helper(map_view);
}

void Controller::close_sailing_view() {
    if (!sailing_view)
        throw Error("Sailing data view is not open!");
    sailing_view = nullptr;
    close_view_helper(sailing_view);
}

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

void Controller::view_default_cmd() {
    if_map_view_closed_error();
    map_view->set_defaults();
}

void Controller::view_size_cmd() {
    if_map_view_closed_error();
    int size = get_int_from_cin();
    map_view->set_size(size);
}

void Controller::view_zoom_cmd() {
    if_map_view_closed_error();
    double scale = get_double_from_cin();
    map_view->set_scale(scale);
}

void Controller::view_pan_cmd() {
    if_map_view_closed_error();
    double x = get_double_from_cin();
    double y = get_double_from_cin();
    map_view->set_origin({x, y});
}

void Controller::view_show_cmd() {
    for_each(all_views.begin(), all_views.end(),
            bind(&View::draw, _1));
}

void Controller::model_status_cmd() {
    Model::get_Instance()->describe();
}

void Controller::model_go_cmd() {
    Model::get_Instance()->update();
}

void Controller::model_create_cmd() {
    string ship_name, ship_type;
    cin >> ship_name;
    if (ship_name.length() < 2) // TODO magic number
        throw Error("Name is too short!");

    Model *model_ptr = Model::get_Instance();
    if (model_ptr->is_name_in_use(ship_name))
        throw Error("Name is invalid!");
    cin >> ship_type;
    Point point = get_point_from_cin();

    shared_ptr<Ship> ship = create_ship(ship_name, ship_type, point);
    model_ptr->add_ship(ship);
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
    shared_ptr<Ship> ship_to_attack =
            Model::get_Instance()->get_ship_ptr(ship_name);
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
    cout << "Done" << endl;
}

void Controller::if_map_view_closed_error() const {
    if (!map_view)
        throw Error("Map view is not open!");
}

void Controller::open_view_helper(std::shared_ptr<View> view) {
    all_views.push_back(view);
    Model::get_Instance()->attach(view);
}

void Controller::close_view_helper(std::shared_ptr<View> view) {
    Model::get_Instance()->detach(view);
    all_views.erase(remove(all_views.begin(), all_views.end(), view),
            all_views.end());
}