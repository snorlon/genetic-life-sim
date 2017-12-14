/*
 * PopulationManager.cpp
 *
 *  Created on: Nov 27, 2017
 *      Author: root
 */

#include <iostream>
using namespace std;

#include "PopulationManager.h"

PopulationManager::PopulationManager(int seed) {
	srand (seed);

	this->geneticPool = NULL;
	this->tick = 0;
	this->tickLimit = 0;
	this->geneticsInitialized = false;
	this->evolutionInitialized = false;
}

PopulationManager::~PopulationManager() {
	// TODO Auto-generated destructor stub
}

void PopulationManager::initializeGenetics(int organismsUsed, double mutationRate,
			float percentPlant, float percentHerbivore, float percentCarnivore,
			float percentRandom, bool weightedBreeding) {

	if(percentPlant + percentHerbivore + percentCarnivore > 1.00) {
		cout<<"Percents exceed 100% for plant/herbivore/carnivore!"<<endl;
		return;
	}
	if(percentRandom > 1.00) {
		cout<<"Random percent cannot exceed 100%!"<<endl;
		return;
	}

	//set some variables for the simulation
	this->poolSize = organismsUsed;
	this->mutationRate = mutationRate;
	this->percentRandom = percentRandom;
	this->weightedBreeding = weightedBreeding;
	this->originalDistribution[0] = percentPlant;
	this->originalDistribution[1] = percentHerbivore;
	this->originalDistribution[2] = percentCarnivore;
	this->originalDistribution[3] = 1-(percentPlant + percentHerbivore + percentCarnivore);
	this->originalDistribution[4] = percentRandom;

	//generate our initial organism data
	this->geneticPool = new Organism[this->poolSize];
	for(unsigned int index=0; index<this->poolSize; index++) {
		//initialize them with a range of random values
		this->geneticPool[index].initializeRandom(70, 30);
	}

	this->originalCount[0] = this->countPlants();
	this->originalCount[1] = this->countHerbivores();
	this->originalCount[2] = this->countCarnivores();
	this->originalCount[3] = this->countOmnivores();

	this->geneticsInitialized = true;
}

void PopulationManager::initializeEvolution(int tickLimit) {
	this->tickLimit = tickLimit;

	this->evolutionInitialized = true;
}

void PopulationManager::run() {
	if(!this->geneticsInitialized) {
		cout<<"Genetics were not initialized!"<<endl;
		return;
	}

	if(!this->evolutionInitialized) {
		cout<<"Evolution was not initialized!"<<endl;
		return;
	}

	while(this->tick < this->tickLimit || this->tickLimit == 0) {

		this->tickTurn();

		this->tick = this->tick + 1;

		if(this->tick % 50 == 0 || this->tick == 1) {
			cout<<"Tick " << this->tick <<": |"<<this->countPlants()<<"|"<<this->countHerbivores()<<"|"
					<<this->countCarnivores()<<"|"<<this->countOmnivores()<<"|" << endl;
		}
	}
	cout<<"Simulation has reached tick limit."<<endl;

	cout<<"Ending simulation with "<<this->countPlants()<<" from "<<this->originalCount[0]<<" plants."<<endl;
	cout<<"Ending simulation with "<<this->countHerbivores()<<" from "<<this->originalCount[1]<<" herbivores."<<endl;
	cout<<"Ending simulation with "<<this->countCarnivores()<<" from "<<this->originalCount[2]<<" carnivores."<<endl;
	cout<<"Ending simulation with "<<this->countOmnivores()<<" from "<<this->originalCount[3]<<" omnivores."<<endl;

	this->end();
}

void PopulationManager::end() {
	//Output results here, such as population changes, organisms consumed, organisms killed, and genetic distribution across population

	delete[] this->geneticPool;
	this->geneticPool = NULL;
}

void PopulationManager::tickTurn() {
	//clear prior cache data
	this->plantLeastTough.clear();
	this->creatureLeastTough.clear();
	this->creatureLeastAgile.clear();
	this->creatureLeastSmart.clear();

	this->creatureTurnOrder.clear();

	//populate caches for plants, creatures
	//this->plantLeastTough.push_back(X);
	for(unsigned int i=0; i < poolSize; i++) {
		//push separately for plants and creatures
		if(this->geneticPool[i].archtype == plant) {
			//plants get pushed to 1 vector
			this->plantLeastTough.push_back(&this->geneticPool[i]);
		}
		else {
			//creatures get pushed to 3 vectors
			this->creatureLeastTough.push_back(&this->geneticPool[i]);
			this->creatureLeastAgile.push_back(&this->geneticPool[i]);
			this->creatureLeastSmart.push_back(&this->geneticPool[i]);
			this->creatureTurnOrder.push_back(&this->geneticPool[i]);
		}
	}

	//sort caches by stat
	sort(this->plantLeastTough.begin(), this->plantLeastTough.end(), sortByTough);
	sort(this->creatureLeastTough.begin(), this->creatureLeastTough.end(), sortByTough);
	sort(this->creatureLeastAgile.begin(), this->creatureLeastAgile.end(), sortByAgile);
	sort(this->creatureLeastSmart.begin(), this->creatureLeastSmart.end(), sortByIntelligence);

	//sort turn order by stat total
	sort(this->creatureTurnOrder.begin(), this->creatureTurnOrder.end(), sortTurnOrder);

	//grant each creature an attack cycle
	for(unsigned int i=0; i < poolSize; i++) {
		ArchType creatureArchtype = this->geneticPool[i].archtype;
		Organism* rolledTarget = NULL;

		switch(creatureArchtype) {
		case plant:
			//plants can't attack :) they live by outreproducing mammals
			break;
		case herbivore:
			//picks a random plant out of all plants, either lives or dies on this choice
			rolledTarget = this->plantLeastTough.at(0);

			cout<< rolledTarget->getValue(Toughness, false) <<endl;
			//target dies if pass
			break;
		case carnivore:
			//grabs weakest 10(PARAMETER?) of each stat, excluding self and those stronger than us in that stat
			//picks randomly from these on whom to attack
			//lives or dies on killing target
			//target dies if pass
			break;
		case omnivore:
			//pools 5(PARAMETER?) weakest of each stat in meat, and 15(PARAMETER?) weakest plants
			//gets to attempt both, plant first, animal if plant fails
			//dies if both fail
			//target dies if pass
			break;
		}
	}
	//creature.attack(creature2);

	//clear the corpses

	//populate caches for plants, herbivore+omnivore, carnivore+omnivore
	//this->plantLeastTough.push_back(Y);

	//grant a breed cycle between random creatures in groups, proportional to breeding pool sizes
	//vector.push_back(creature.breed(creature));
}



int PopulationManager::countPlants() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(this->geneticPool[i].archtype == plant) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countHerbivores() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(this->geneticPool[i].archtype == herbivore) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countCarnivores() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(this->geneticPool[i].archtype == carnivore) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countOmnivores() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(this->geneticPool[i].archtype == omnivore) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countAnimals() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(this->geneticPool[i].archtype != plant) {
			count++;
		}
	}

	return count;
}

bool PopulationManager::sortByTough(Organism* i, Organism* j) {
	return j->stronger(i, Toughness, false);
}

bool PopulationManager::sortByAgile(Organism* i, Organism* j) {
	return j->stronger(i, Agility, false);
}

bool PopulationManager::sortByIntelligence(Organism* i, Organism* j) {
	return j->stronger(i, Intelligence, false);
}

bool PopulationManager::sortTurnOrder(Organism* i, Organism* j) {
	float total_i = i->getStatTotal();
	float total_j = j->getStatTotal();
	return total_i < total_j;
}
