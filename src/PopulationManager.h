/*
 * PopulationManager.h
 *
 *	Will handle creating, running, and breeding of genetic data
 *	Will handle eating, attacking, extinction, and breeding on individual species on a per-turn basis
 *	Will provide diagnostics at intervals for current world status
 *
 *  Created on: Nov 27, 2017
 *      Author: Erin Brown
 */

#ifndef POPULATIONMANAGER_H_
#define POPULATIONMANAGER_H_

#include "Organism.h"

#include <algorithm>
#include <vector>
#include <map>
#include "support/Config.h"
#include "support/Parameters.h"

const int tickInfoFrequency = 1;
const int chanceToFailToBreed = 50;//%, if failure, corpse from prior cycle remains
	//this adds an element of corpses to the food chain and population density

class PopulationManager {
public:
	PopulationManager(Config* simConfig, Parameters* simParams, vector<Organism*> &templateOrganismTypes, int seed = time(NULL));
	virtual ~PopulationManager();

	void initializeGenetics();

	void run();

	void end();

	void tickTurn();
	void attack();
	void eat();
	void reproduce();

	int countPlants();
	int countAnimals();
	int countMushrooms();
	int countSpecies(string speciesName);
	int countNonSpecies(string speciesName);

	//Function will take a maximum range and flip coins on each index to decide if we "take" it, thus leading to exponentially less chance on each higher index
	int getWeightedIndex(unsigned int maxIndex, unsigned int minIndex = 0);

	Organism* getWeightedWeakest(vector<Organism*> &possibleTargets, unsigned int maxWeakestPullable = 999999);

	Organism* pickRandomTemplate();

	//dead end up at back of the line in these sorts, with the weakest living in the front
	static bool sortByTough(Organism* i, Organism* j);
	static bool sortByAgile(Organism* i, Organism* j);
	static bool sortByIntelligence(Organism* i, Organism* j);

	//sorted by "size"
	static bool sortTurnOrder(Organism* i, Organism* j);

private:
	Organism* geneticPool;

	vector<Organism*> organismTemplates;
	std::map<string, int> templateIndexes;

	Config* simConfig;
	Parameters* simParams;

	//stores a list of each creature group separately from the genetic pool
	//get list by index of species in template list
	//sorted by food amount when breeding cycle comes to pass
	//sorted by their weakest living for attack cycles, with their dead at the back
	//during attack cycles, their weakest will be copied to another vector of prey to hunt
	vector<vector<Organism*> > speciesGroups;

	vector<Organism*> creatureTurnOrder;

	int tick;
	int tickLimit;

	unsigned int poolSize;
	vector<float> originalDistribution;

	bool geneticsInitialized;

	int cachedTotalProbability;
};

#endif /* POPULATIONMANAGER_H_ */
