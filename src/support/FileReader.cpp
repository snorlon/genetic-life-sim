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

}

void FileReader::readConfig(Config* config) {
	// TODO: load in config data to config file
}

vector<Organism*> FileReader::readOrganisms(string filename) {
	vector<Organism*> loadedOrganismClasses;
	loadedOrganismClasses.clear();

	ifstream file;
	file.open(filename.c_str());
	string parameter;
	parameter.clear();
	string value;
	value.clear();

	Organism* newOrganism = NULL;

	while(file >> parameter) {
		file >> value;
		if(parameter.compare("Organism:") == 0) {
			newOrganism = new Organism();
			newOrganism->name = value;
			loadedOrganismClasses.push_back(newOrganism);
		}
		else if(parameter.compare("Symbol:") == 0) {
			newOrganism->symbol = value;
		}
		else if(parameter.compare("Type:") == 0) {
			if(value.compare("Plant") == 0) {
				newOrganism->archtype = plant;
			}
			else if(value.compare("Animal") == 0) {
				newOrganism->archtype = animal;
			}
			else if(value.compare("Fungus") == 0) {
				newOrganism->archtype = fungus;
			}
		}
		else if(parameter.compare("EatsPlants:") == 0) {
			newOrganism->eatsPlants = value.compare("true");
		}
		else if(parameter.compare("EatsAnimals:") == 0) {
			newOrganism->eatsAnimals = value.compare("true");
		}
		else if(parameter.compare("EatsFungus:") == 0) {
			newOrganism->eatsFungus = value.compare("true");
		}
		else if(parameter.compare("EatsDead:") == 0) {
			newOrganism->eatsDead = value.compare("true");
		}
		else if(parameter.compare("Cannibal:") == 0) {
			newOrganism->cannibal = value.compare("true");
		}
		else if(parameter.compare("SuddenDeathChance:") == 0) {
			newOrganism->suddenDeathChance = atof(value.c_str());
		}
		else if(parameter.compare("MutationRate:") == 0) {
			newOrganism->mutationRate = atoi(value.c_str());
		}
		else if(parameter.compare("ToughnessMin:") == 0) {
			newOrganism->toughnessMin = atoi(value.c_str());
		}
		else if(parameter.compare("ToughnessMax:") == 0) {
			newOrganism->toughnessMax = atoi(value.c_str());
		}
		else if(parameter.compare("ToughnessVarianceMin:") == 0) {
			newOrganism->toughnessVarianceMin = atoi(value.c_str());
		}
		else if(parameter.compare("ToughnessVarianceMax:") == 0) {
			newOrganism->toughnessVarianceMax = atoi(value.c_str());
		}
		else if(parameter.compare("AgilityMin:") == 0) {
			newOrganism->agilityMin = atoi(value.c_str());
		}
		else if(parameter.compare("AgilityMax:") == 0) {
			newOrganism->agilityMax = atoi(value.c_str());
		}
		else if(parameter.compare("AgilityVarianceMin:") == 0) {
			newOrganism->agilityVarianceMin = atoi(value.c_str());
		}
		else if(parameter.compare("AgilityVarianceMax:") == 0) {
			newOrganism->agilityVarianceMax = atoi(value.c_str());
		}
		else if(parameter.compare("IntelligenceMin:") == 0) {
			newOrganism->intelligenceMin = atoi(value.c_str());
		}
		else if(parameter.compare("IntelligenceMax:") == 0) {
			newOrganism->intelligenceMax = atoi(value.c_str());
		}
		else if(parameter.compare("IntelligenceVarianceMin:") == 0) {
			newOrganism->intelligenceVarianceMin = atoi(value.c_str());
		}
		else if(parameter.compare("IntelligenceVarianceMax:") == 0) {
			newOrganism->intelligenceVarianceMax = atoi(value.c_str());
		}
		else if(parameter.compare("BreedThresholdMultiplier:") == 0) {
			newOrganism->breedThreshold = atof(value.c_str());
		}
		else if(parameter.compare("FoodStatPenalty:") == 0) {
			newOrganism->foodStatPenalty = atof(value.c_str());
		}
		else if(parameter.compare("FoodStatVariancePenalty:") == 0) {
			newOrganism->foodStatVariancePenalty = atof(value.c_str());
		}
		else if(parameter.compare("FoodProductionRate:") == 0) {
			newOrganism->foodProductionRate = atof(value.c_str());
		}
		else if(parameter.compare("FoodCapMultiplier:") == 0) {
			newOrganism->foodCapMultiplier = atof(value.c_str());
		}
	}

	file.close();

	cout<<"Loaded "<<loadedOrganismClasses.size()<<" organism templates."<<endl;

	return loadedOrganismClasses;
}
