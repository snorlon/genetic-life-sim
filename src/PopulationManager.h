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
#include "support/Config.h"
#include "support/Parameters.h"

const int tickInfoFrequency = 100;
const int chanceToFailToBreed = 50;//%, if failure, corpse from prior cycle remains
	//this adds an element of corpses to the food chain and population density

class PopulationManager {
public:
	PopulationManager(Config* simConfig, Parameters* simParams, int seed = time(NULL));
	virtual ~PopulationManager();

	void initializeGenetics();

	void run();

	void end();

	void tickTurn();
	void attack();
	void eat();
	void reproduce();

	int countPlants();
	int countHerbivores();
	int countCarnivores();
	int countOmnivores();
	int countAnimals();

	//Function will take in a count of 1 to MAX_WEIGHTS and return an index in that range -1
	int getWeightedIndex(unsigned int possibleCount);

	Organism* getWeightedWeakest(vector<Organism*> &possibleTargets, unsigned int maxWeakestPullable, unsigned int &index);

	static bool sortByTough(Organism* i, Organism* j);
	static bool sortByAgile(Organism* i, Organism* j);
	static bool sortByIntelligence(Organism* i, Organism* j);
	static bool sortTurnOrder(Organism* i, Organism* j);

	static const unsigned int MAX_WEIGHTS = 100;

private:
	Organism* geneticPool;

	Config* simConfig;
	Parameters* simParams;

	unsigned int weightedTotal [MAX_WEIGHTS];

	//Plants only have a toughness factor, as they aren't agile or smart in any way
	//This will lead to herbivores having a toughness-based approach to evolution, which is natural in real life
	vector<Organism*> plantLeastTough;

	vector<Organism*> creatureLeastTough;
	vector<Organism*> creatureLeastAgile;
	vector<Organism*> creatureLeastSmart;

	vector<Organism*> creatureTurnOrder;

	int tick;
	int tickLimit;

	unsigned int poolSize;
	float percentRandom;
	float originalDistribution[4];
	int originalCount[4];

	bool geneticsInitialized;
	bool evolutionInitialized;
};

#endif /* POPULATIONMANAGER_H_ */
