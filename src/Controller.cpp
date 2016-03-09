#include "Controller.h"

#include "Model.h"
#include "Utility.h"
#include "View.h"

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

void Controller::view_default_cmd() {
    view_ptr->set_defaults();
}

void Controller::view_size_cmd() {
    int size;
    if (!(cin >> size))
        throw Error("Expected an integer!");
    view_ptr->set_size(size);
}

void Controller::view_zoom_cmd() {
    double scale;
    if (!(cin >> scale))
        throw Error("Expected a double!");
    view_ptr->set_scale(scale);
}

void Controller::view_pan_cmd() {
    double x, y;
    if (!(cin >> x >> y))
        throw Error("Expected a double!");
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

}

void Controller::ship_course_cmd(Ship* ship) {

}

void Controller::ship_position_cmd(Ship* ship) {

}

void Controller::ship_dest_cmd(Ship* ship) {

}

void Controller::ship_load_cmd(Ship* ship) {

}

void Controller::ship_unload_cmd(Ship* ship) {

}

void Controller::ship_dock_cmd(Ship* ship) {

}

void Controller::ship_attack_cmd(Ship* ship) {

}

void Controller::ship_refuel_cmd(Ship* ship) {

}

void Controller::ship_stop_cmd(Ship* ship) {

}

void Controller::ship_stop_attack_cmd(Ship* ship) {

}
