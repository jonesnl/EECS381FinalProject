#ifndef CONTROLLER_H
#define CONTROLLER_H
/* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/

#include <map>
#include <string>
#include <memory>
#include <list>

class View;
class MapView;
class SailingView;
class BridgeView;
class Ship;

class Controller {
public:
    // create View object, run the program by acccepting user commands, then destroy View object
    void run();

private:
    // Controller keeps its own pointer to the View because it has to manage the View.
    // Future versions will need to manage more than one view.
    std::shared_ptr<MapView> map_view;
    std::shared_ptr<SailingView> sailing_view;
    std::map<std::string, std::shared_ptr<BridgeView>> bridge_view_map;
    std::list<std::shared_ptr<View>> all_views;

    // Helper commands called in run()
    void open_map_view();
    void open_sailing_view();
    void open_bridge_view();
    void close_map_view();
    void close_sailing_view();
    void close_bridge_view();
    void view_default_cmd();
    void view_size_cmd();
    void view_zoom_cmd();
    void view_pan_cmd();
    void view_show_cmd();

    void model_status_cmd();
    void model_go_cmd();
    void model_create_cmd();

    void ship_course_cmd(std::shared_ptr<Ship> ship);
    void ship_position_cmd(std::shared_ptr<Ship> ship);
    void ship_dest_cmd(std::shared_ptr<Ship> ship);
    void ship_load_cmd(std::shared_ptr<Ship> ship);
    void ship_unload_cmd(std::shared_ptr<Ship> ship);
    void ship_dock_cmd(std::shared_ptr<Ship> ship);
    void ship_attack_cmd(std::shared_ptr<Ship> ship);
    void ship_refuel_cmd(std::shared_ptr<Ship> ship);
    void ship_stop_cmd(std::shared_ptr<Ship> ship);
    void ship_stop_attack_cmd(std::shared_ptr<Ship> ship);

    void quit_helper();

    void if_map_view_closed_error() const;
    void open_view_helper(std::shared_ptr<View> view);
    void close_view_helper(std::shared_ptr<View> view);
};

#endif