#include "test_utility.h"
#include "../View.h"
#include "../Model.h"

#include <iostream>

using namespace std;

void set_fp_cout() {
    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);
    g_Model_ptr = new Model();
    View *v = new View();
    g_Model_ptr->attach(v);
}
