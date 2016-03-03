#include "test_utility.h"

#include <iostream>

using namespace std;

void set_fp_cout() {
    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);
}
