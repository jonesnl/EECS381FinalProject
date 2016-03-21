#ifndef CONTROLLER_H
#define CONTROLLER_H
/* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/

#include <map>
#include <string>
#include <memory>

class View;
class Ship;

class Controller {
public:
    // output constructor message
    Controller();

    // output destructor message
    ~Controller();

    // create View object, run the program by acccepting user commands, then destroy View object
    void run();

private:
    // Controller keeps its own pointer to the View because it has to manage the View.
    // Future versions will need to manage more than one view.
    std::shared_ptr<View> view_ptr;

    // Helper commands called in run()
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
};

#endif