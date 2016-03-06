#ifndef CONTROLLER_H
#define CONTROLLER_H
/* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/

#include <map>
#include <string>

/* 
*** This skeleton file shows the required public and protected interface for the class, which you may not modify. 
If any protected or private members are shown here, then your class must also have them and use them as intended.
You should delete this comment.
*/
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
	View *view_ptr;

    void view_default_cmd();
    void view_size_cmd();
    void view_zoom_cmd();
    void view_pan_cmd();
    void view_show_cmd();

    void model_status_cmd();
    void model_go_cmd();
    void model_create_cmd();

    void ship_course_cmd(Ship* ship);
    void ship_position_cmd(Ship* ship);
    void ship_dest_cmd(Ship* ship);
    void ship_load_cmd(Ship* ship);
    void ship_unload_cmd(Ship* ship);
    void ship_dock_cmd(Ship* ship);
    void ship_attack_cmd(Ship* ship);
    void ship_refuel_cmd(Ship* ship);
    void ship_stop_cmd(Ship* ship);
    void ship_stop_attack_cmd(Ship* ship);
};

#endif