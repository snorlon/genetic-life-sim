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
	this->initializeClass((ArchType)(rand() % 4));
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
		float roll = rand();
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

float Organism::getStatTotal() {
	return this->getValue(Toughness, false) + this->getValue(Agility, false) + this->getValue(Intelligence, false);
}
