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
#include "support/Statistics.h"
#include "support/FileReader.h"

#include <stdio.h>



int main() {
	Config simConfig;
	Parameters simParams;
	FileReader simFileReader;
	Statistics simStats;
	vector<Organism*> templateOrganismTypes;

	simFileReader.readParameters(&simParams);

	//Creates a vector of organism designs from the file
	templateOrganismTypes = simFileReader.readOrganisms();
	if(templateOrganismTypes.size() <= 0) {
		cout<<"Critical error! No organism templates loaded! Aborting execution."<<endl;
		return -1;
	}

	PopulationManager popManager(&simConfig, &simParams, &simStats, templateOrganismTypes);

	popManager.initializeGenetics();
	popManager.run();

	return 0;
}
