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

class PopulationManager {
public:
	PopulationManager(int seed = time(NULL));
	virtual ~PopulationManager();

	//Mutation rate is 0.00-1.00 percent variation added per iteration per gene
	//Omnivores fill out the remaining percent
	//Randoms will not eat into percent used by original organisms, ex: 0.3 random = 30% of total is random, other 70% divides for original
	void initializeGenetics(int organismsUsed, double mutationRate,
			float percentPlant, float percentHerbivore, float percentCarnivore,
			float percentRandom = 0.0, bool weightedBreeding = true);

	void initializeEvolution(int tickLimit);

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
	float mutationRate;
	float originalDistribution[4];
	int originalCount[4];
	bool weightedBreeding;

	bool geneticsInitialized;
	bool evolutionInitialized;
};

#endif /* POPULATIONMANAGER_H_ */
