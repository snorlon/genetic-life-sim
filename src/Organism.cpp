/*
 * Organism.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: root
 */

#include "Organism.h"
#include <stdlib.h>

Organism::Organism() {
	dead = false;

	foodConsumption = 0;
	foodCap = 5;

	food = 0;

	foodStatPenalty = 0.01;
	foodStatVariancePenalty = 0.01;
	foodProductionRate = 0;
	foodCapMultiplier = 1.0;

	toughnessMin = 0;
	toughnessMax = 1;
	toughnessVarianceMin = 0;
	toughnessVarianceMax = 0;
	agilityMin = 0;
	agilityMax = 1;
	agilityVarianceMin = 0;
	agilityVarianceMax = 1;
	intelligenceMin = 0;
	intelligenceMax = 1;
	intelligenceVarianceMin = 0;
	intelligenceVarianceMax = 1;

	//initial parameters

	agility = 0;
	agilityVariance = 0;
	toughness = 0;
	toughnessVariance = 0;
	intelligence = 0;
	intelligenceVariance = 0;

	kills = 0;

	archtype = plant;

	symbol = ' ';

	eatsPlants = false;
	eatsAnimals = false;
	eatsFungus = false;
	eatsDead = false;

	cannibal = false;

	breedThreshold = 1.0;

	suddenDeathChance = 0.00;

	mutationRate = 1;
}

Organism::~Organism() {
	// TODO Auto-generated destructor stub
}

void Organism::instantiate(Organism* &organism_template) {
	dead = false;

	foodStatPenalty = organism_template->foodStatPenalty;
	foodStatVariancePenalty = organism_template->foodStatVariancePenalty;
	foodProductionRate = organism_template->foodProductionRate;
	foodCapMultiplier = organism_template->foodCapMultiplier;

	//initial parameters

	agility = randomStatInRange(organism_template->agilityMin, organism_template->agilityMax);
	agilityVariance = randomStatInRange(organism_template->agilityVarianceMin, organism_template->agilityVarianceMax);
	toughness = randomStatInRange(organism_template->toughnessMin, organism_template->toughnessMax);
	toughnessVariance = randomStatInRange(organism_template->toughnessVarianceMin, organism_template->toughnessVarianceMax);
	intelligence = randomStatInRange(organism_template->intelligenceMin, organism_template->intelligenceMax);
	intelligenceVariance = randomStatInRange(organism_template->intelligenceVarianceMin, organism_template->intelligenceVarianceMax);

	kills = 0;

	archtype = organism_template->archtype;

	symbol = organism_template->symbol;

	eatsPlants = organism_template->eatsPlants;
	eatsAnimals = organism_template->eatsAnimals;
	eatsFungus = organism_template->eatsFungus;
	eatsDead = organism_template->eatsDead;

	cannibal = organism_template->cannibal;

	breedThreshold = organism_template->breedThreshold;

	suddenDeathChance = organism_template->suddenDeathChance;

	mutationRate = organism_template->mutationRate;

	recalculateFood();
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

		food += prey->food * divisor;
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

		food += foodGain * divisor;
	}
}

void Organism::consumeFood() {
	food -= foodConsumption;
	if(food < 0) {
		dead = true;
	}
}

void Organism::initialize(int ntoughness, int nagility, int nintelligence,
		int ntoughnessVariance, int nagilityVariance, int nintelligenceVariance) {
	dead = false;
	toughness = ntoughness;
	agility = nagility;
	intelligence = nintelligence;
	toughnessVariance = ntoughnessVariance;
	agilityVariance = nagilityVariance;
	intelligenceVariance = nintelligenceVariance;

	//cap variance to not exceed initial value
	if(toughnessVariance > toughness) {
		toughnessVariance = toughness;
	}
	if(agilityVariance > agility) {
		agilityVariance = agility;
	}
	if(intelligenceVariance > intelligence) {
		intelligenceVariance = intelligence;
	}

	recalculateFood();
}

void Organism::initializeClass(ArchType narchtype) {
	archtype = narchtype;

	if(archtype == plant) {
		agility = 0;
		intelligence = 0;
		agilityVariance = 0;
		intelligenceVariance = 0;
	}


	recalculateFood();
}

void Organism::initializeRandom(int maxStatRange, int maxStatVarianceRange) {
	initialize((rand() % maxStatRange)+1, (rand() % maxStatRange)+1, (rand() % maxStatRange)+1,
			(rand() % maxStatVarianceRange)+1, (rand() % maxStatVarianceRange)+1, (rand() % maxStatVarianceRange)+1);
}


float Organism::getValue(Stat stat, bool rollCheck) {
	float val = 0;
	float maxRange = 0;
	switch(stat) {
	case Toughness:
		val = toughness;
		maxRange = toughnessVariance;
		break;
	case Agility:
		val = agility;
		maxRange = agilityVariance;
		break;
	case Intelligence:
		val = intelligence;
		maxRange = intelligenceVariance;
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

	float ourStat = getValue(stat, rollCheck);
	float theirStat = target->getValue(stat, rollCheck) * multiplier;

	return (ourStat > theirStat);
}

void Organism::beBorn(Organism* parent1, Organism* parent2) {
	dead = false;
	archtype = parent1->archtype;

	toughness = mutateStat(randomStatInRange(parent1->toughness, parent2->toughness));
	agility = mutateStat(randomStatInRange(parent1->agility, parent2->agility));
	intelligence = mutateStat(randomStatInRange(parent1->intelligence, parent2->intelligence));


	toughnessVariance = mutateStat(randomStatInRange(parent1->toughnessVariance, parent2->toughnessVariance));
	agilityVariance = mutateStat(randomStatInRange(parent1->agilityVariance, parent2->agilityVariance));
	intelligenceVariance = mutateStat(randomStatInRange(parent1->intelligenceVariance, parent2->intelligenceVariance));
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

int Organism::mutateStat(int value) {
	int mutationAmount = (rand() % (mutationRate*2)) - mutationRate;

	int newValue = value + mutationAmount;

	if(newValue < 0) {
		return 0;
	}

	return newValue;
}

float Organism::getStatTotal() {
	return getValue(Toughness, false) + getValue(Agility, false) + getValue(Intelligence, false);
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
	foodConsumption = (agility + intelligence + toughness)*0.03 +
			(agilityVariance + intelligenceVariance + toughnessVariance)*0.05;


	//base foodcap on consumption
	foodCap = foodConsumption*2.1;
}
