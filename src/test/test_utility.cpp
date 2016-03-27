#include "test_utility.h"
#include "../View.h"
#include "../Model.h"
#include "../Views.h"

#include <iostream>

using namespace std;

void set_fp_cout() {
    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);
    shared_ptr<View> v = make_shared<MapView>();
    Model::get_inst()->attach(v);
}
