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
	int countHerbivores();
	int countCarnivores();
	int countOmnivores();
	int countAnimals();

	//Function will take a maximum range and flip coins on each index to decide if we "take" it, thus leading to exponentially less chance on each higher index
	int getWeightedIndex(unsigned int maxIndex, unsigned int minIndex = 0);

	Organism* getWeightedWeakest(vector<Organism*> &possibleTargets, unsigned int maxWeakestPullable, unsigned int &index);

	Organism* pickRandomTemplate();

	static bool sortByTough(Organism* i, Organism* j);
	static bool sortByAgile(Organism* i, Organism* j);
	static bool sortByIntelligence(Organism* i, Organism* j);
	static bool sortTurnOrder(Organism* i, Organism* j);

private:
	Organism* geneticPool;

	vector<Organism*> organismTemplates;

	Config* simConfig;
	Parameters* simParams;

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
	vector<float> originalDistribution;

	bool geneticsInitialized;
	bool evolutionInitialized;

	int cachedTotalProbability;
};

#endif /* POPULATIONMANAGER_H_ */
