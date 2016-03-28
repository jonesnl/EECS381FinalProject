#include "View.h"

using namespace std;

// Empty function definitions to allow derived views to ignore certain data streams
// by simply not overriding these functions.

void View::update_location(const std::string&, Point) { }
void View::update_course(const std::string&, double) { }
void View::update_speed(const std::string&, double) { }
void View::update_fuel(const std::string&, double) { }
void View::update_remove(const std::string&) { }