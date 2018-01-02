//============================================================================
// Name        : MyLittleWorld.cpp
// Author      : Erin Brown
// Version     :
// Copyright   : No copypastarino!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "PopulationManager.h"

#include <stdio.h>



int main() {
	PopulationManager popManager;

	popManager.initializeGenetics(50000, 0.1, 0.5, 0.3, 0.1, 0.1);
	popManager.initializeEvolution(10000);
	popManager.run();

	//cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
