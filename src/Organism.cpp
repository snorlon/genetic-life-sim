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

	this->foodConsumption = 0;
	this->foodCap = 5;

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

void Organism::eat(Organism* prey) {
	//consume prey's food if not a plant
	if(archtype != plant) {
		int currFood = food;
		//determine how much we're taking from target, limited by our stomach size
		int foodConsumed = prey->food;
		if(foodConsumed > foodCap) {
			foodConsumed = foodCap;
		}

		//remove the food consumed from the creature so that it can try to not die ;)
		prey->food -= foodConsumed;

		//default food loss multiplier
		float divisor = 0.1;
		//default decay rate
		int decayRate = 500;

		switch(archtype) {
		case herbivore:
			divisor = herbivoreConsumptionGain;
			decayRate = herbivoreFoodGainDecay;
			break;
		case carnivore:
			divisor = carnivoreConsumptionGain;
			decayRate = carnivoreFoodGainDecay;
			break;
		case omnivore:
			divisor = omnivoreConsumptionGain;
			decayRate = omnivoreFoodGainDecay;
			break;
		default:
			break;
		}

		//for each 'decayRate' food we have, we divide what we consume more
		while(currFood > decayRate) {
			divisor /= 2;
			currFood -= decayRate;
		}

		this->food += prey->food * divisor;
	} else {
		//otherwise, generate food based on our current size
		float statDifference = productionRateComparisonUpperbound - getStatTotal();

		//prevent negative values
		if(statDifference < 0) {
			statDifference = 0;
		}

		int foodGain = statDifference * baseProductionRate;
		int currFood = food;
		float divisor = 1.0;

		//apply decay on all gained food
		while(currFood> plantFoodGainDecay) {
			currFood -= plantFoodGainDecay;
			divisor /= 2;
		}

		this->food += foodGain * divisor;
	}
}

void Organism::consumeFood() {
	food -= foodConsumption;
	if(food < 0) {
		dead = true;
	}
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

	recalculateFood();
}

void Organism::initializeClass(ArchType archtype) {
	this->archtype = archtype;

	if(this->archtype == plant) {
		this->agility = 0;
		this->intelligence = 0;
		this->agilityVariance = 0;
		this->intelligenceVariance = 0;
	}


	recalculateFood();
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

	float multiplier = 1.0;
	//plants take penalty vs every organism, herbivores take penalty vs carnivores and omnivores
	//carnivores take advantage defensively vs other carnivores and omnivores
	//this will create a stronger advantage for archtypes of creatures
	//but only apply during rollCheck, as this is used for actual damage calculations
	if(rollCheck) {
		if(target->archtype == plant) {
			multiplier = 0.8;
		} else if(target->archtype == herbivore) {
			if(archtype == carnivore) {
				multiplier = 0.8;
			} else {
				multiplier = 0.9;
			}
		} else if(target->archtype == carnivore) {
			multiplier = 1.1;
		}
	}

	float ourStat = this->getValue(stat, rollCheck);
	float theirStat = target->getValue(stat, rollCheck) * multiplier;

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

int Organism::getToughness() {
	return toughness;
}

int Organism::getAgility() {
	return agility;
}

int Organism::getIntelligence() {
	return intelligence;
}

void Organism::recalculateFood() {

	//base food consumption on stats
	this->foodConsumption = (this->agility + this->intelligence + this->toughness)*0.03 +
			(this->agilityVariance + this->intelligenceVariance + this->toughnessVariance)*0.05;


	//base foodcap on consumption
	this->foodCap = this->foodConsumption*2.1;
}
