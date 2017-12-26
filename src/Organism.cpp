/*
 * Organism.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: root
 */

#include "Organism.h"
#include <stdlib.h>

Organism::Organism() {
	//temp!!
	this->food = (rand() % 100) + 1;
	this->dead = false;

	this->cachedFoodConsumption = 0;

	//initial parameters

	this->agility = 0;
	this->agilityVariance = 0;
	this->toughness = 0;
	this->toughnessVariance = 0;
	this->intelligence = 0;
	this->intelligenceVariance = 0;

	this->kills = 0;

	this->archtype = plant;
}

Organism::~Organism() {
	// TODO Auto-generated destructor stub
}

void Organism::tickTurn() {

}

void Organism::initialize(int toughness, int agility, int intelligence,
		int toughnessVariance, int agilityVariance, int intelligenceVariance) {
	this->dead = false;
	this->toughness = toughness;
	this->agility = agility;
	this->intelligence = intelligence;
	this->toughnessVariance = toughnessVariance;
	this->agilityVariance = agilityVariance;
	this->intelligenceVariance = intelligenceVariance;

	//cap variance to not exceed initial value
	if(this->toughnessVariance > this->toughness) {
		this->toughnessVariance = this->toughness;
	}
	if(this->agilityVariance > this->agility) {
		this->agilityVariance = this->agility;
	}
	if(this->intelligenceVariance > this->intelligence) {
		this->intelligenceVariance = this->intelligence;
	}
}

void Organism::initializeClass(ArchType archtype) {
	this->archtype = archtype;

	if(this->archtype == plant) {
		this->agility = 0;
		this->intelligence = 0;
		this->agilityVariance = 0;
		this->intelligenceVariance = 0;
	}
}

void Organism::initializeRandom(int maxStatRange, int maxStatVarianceRange) {
	this->initialize((rand() % maxStatRange)+1, (rand() % maxStatRange)+1, (rand() % maxStatRange)+1,
			(rand() % maxStatVarianceRange)+1, (rand() % maxStatVarianceRange)+1, (rand() % maxStatVarianceRange)+1);
}


float Organism::getValue(Stat stat, bool rollCheck) {
	float val = 0;
	float maxRange = 0;
	switch(stat) {
	case Toughness:
		val = this->toughness;
		maxRange = this->toughnessVariance;
		break;
	case Agility:
		val = this->agility;
		maxRange = this->agilityVariance;
		break;
	case Intelligence:
		val = this->intelligence;
		maxRange = this->intelligenceVariance;
		break;
	}

	if(rollCheck && maxRange > 0) {
		float newMaxRange = ((rand() % (int)(maxRange * 2)) - maxRange);
		val = val - newMaxRange;

		if(val < 0) {
			val = 0;
		}
	}

	return val;
}

bool Organism::stronger(Organism* target, Stat stat, bool rollCheck) {
	if(target == NULL) {
		return true;
	}

	float ourStat = this->getValue(stat, rollCheck);
	float theirStat = target->getValue(stat, rollCheck);

	return (ourStat > theirStat);
}

void Organism::beBorn(Organism* parent1, Organism* parent2, int mutationRate) {
	dead = false;
	archtype = parent1->archtype;

	toughness = mutateStat(randomStatInRange(parent1->toughness, parent2->toughness), mutationRate);
	agility = mutateStat(randomStatInRange(parent1->agility, parent2->agility), mutationRate);
	intelligence = mutateStat(randomStatInRange(parent1->intelligence, parent2->intelligence), mutationRate);


	toughnessVariance = mutateStat(randomStatInRange(parent1->toughnessVariance, parent2->toughnessVariance), mutationRate);
	agilityVariance = mutateStat(randomStatInRange(parent1->agilityVariance, parent2->agilityVariance), mutationRate);
	intelligenceVariance = mutateStat(randomStatInRange(parent1->intelligenceVariance, parent2->intelligenceVariance), mutationRate);
}

int Organism::randomStatInRange(int val1, int val2) {
	int lower = val1;
	int higher = val2;

	if(lower > val2) {
		lower = val2;
		higher = val1;
	}

	int range = higher - lower;

	if(range <= 0) {
		return lower;
	}

	int rolled = rand() % range;

	return rolled + lower;
}

int Organism::mutateStat(int value, int mutationRate) {
	int mutationAmount = (rand() % (mutationRate*2)) - mutationRate;

	int newValue = value + mutationAmount;

	if(newValue < 0) {
		return 0;
	}

	return newValue;
}

float Organism::getStatTotal() {
	return this->getValue(Toughness, false) + this->getValue(Agility, false) + this->getValue(Intelligence, false);
}
