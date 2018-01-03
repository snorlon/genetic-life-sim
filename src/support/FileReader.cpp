/*
 * FileReader.cpp
 *
 *  Created on: Jan 3, 2018
 *      Author: root
 */

#include "FileReader.h"

#include <iostream>
#include <string>

using namespace std;

FileReader::FileReader() {
	// TODO Auto-generated constructor stub

}

FileReader::~FileReader() {
	// TODO Auto-generated destructor stub
}


void FileReader::readParameters(Parameters* parameters, string filename) {
	ifstream file;
	file.open(filename.c_str());
	string parameter;
	parameter.clear();
	string value;
	value.clear();

	while(file >> parameter) {
		file >> value;
		if(parameter.compare("Population_Cap:") == 0) {
			parameters->populationCap = atoi(value.c_str());
		}
		else if(parameter.compare("Max_Cycles:") == 0) {
			parameters->maxCycleCount = atoi(value.c_str());
		}
		else if(parameter.compare("Stat_Food_Penalty:") == 0) {
			parameters->statFoodPenalty = atof(value.c_str());
		}
		else if(parameter.compare("Stat_Variance_Food_Penalty:") == 0) {
			parameters->statVarianceFoodPenalty = atof(value.c_str());
		}
		else if(parameter.compare("Initial_Percent_Random:") == 0) {
			parameters->initialRandomPercent = atof(value.c_str());
		}
		else if(parameter.compare("Breeding_Percent_Random:") == 0) {
			parameters->randomPerBreedCycle = atof(value.c_str());
		}
		else if(parameter.compare("Spawn_Rate:") == 0) {
			parameters->spawnRates.push_back(atof(value.c_str()));
		}
	}

	file.close();

	//TODO remove this later, will be reusing it for saving output data
	/*ofstream file2;
	file2.open(filename.c_str(),fstream::out | fstream::trunc);
	if(file2.is_open()) {
		cout<<"File made!"<<endl;
		file2 << "Creating file!";
		file2.close();
	}
	else {
		cout<<"OH NO"<<endl;
	}

	file2.close();*/

}

void FileReader::readConfig(Config* config) {
	// TODO: load in config data to config file
}
