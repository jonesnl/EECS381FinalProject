/*
Main module. Your project must use this file as-is
*/

#include "Controller.h"
#include "Model.h"
#include <iostream>

using namespace std;

// The main function creates the Controller object, then tells it to run.

int main ()
{		
	// Set output to show two decimal places
//	cout << fixed << setprecision(2) << endl;
	cout.setf(ios::fixed, ios::floatfield);
	cout.precision(2);
	g_Model_ptr = new Model; // TODO remove
	// create the Controller and go
	Controller controller;

	controller.run();
	delete g_Model_ptr; // TODO remove
}

