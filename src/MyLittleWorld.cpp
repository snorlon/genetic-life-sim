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
#include "support/Config.h"
#include "support/Parameters.h"
#include "support/FileReader.h"

#include <stdio.h>



int main() {
	Config simConfig;
	Parameters simParams;
	FileReader simFileReader;

	simFileReader.readParameters(&simParams);

	PopulationManager popManager(&simConfig, &simParams);

	popManager.initializeGenetics(100000, 0.1, 0.5, 0.3, 0.05, 0.1);
	popManager.initializeEvolution(100000);
	//popManager.run();

	//cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
