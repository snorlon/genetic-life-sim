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

	popManager.initializeGenetics(50000, 0.03, 0.6, 0.1, 0.27);
	popManager.initializeEvolution(50000);
	popManager.run();

	//cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
