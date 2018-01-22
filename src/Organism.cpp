/*
 * Organism.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: root
 */

#include <iostream>

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

	//for future generations
	agilityMin = organism_template->agilityMin;
	agilityVarianceMin = organism_template->agilityVarianceMin;
	toughnessMin = organism_template->toughnessMin;
	toughnessVarianceMin = organism_template->toughnessVarianceMin;
	intelligenceMin = organism_template->intelligenceMin;
	intelligenceVarianceMin = organism_template->intelligenceVarianceMin;
}

void Organism::tickTurn() {

}

void Organism::eat(Organism* prey) {
	//can only consume up to 40% of food from target, remainder on target is halved
	if(prey != NULL) {
		if(prey->food <= 0) {
			//abort if nothing to eat!
			return;
		}

		//determine how much we're taking from target, limited by our stomach size
		int foodConsumed = prey->food * 0.7;
		int foodAvailableSpace = foodCap - food;
		if(foodAvailableSpace < 0) {
			foodAvailableSpace = 0;
		}

		if(foodConsumed > foodAvailableSpace) {
			foodConsumed = foodAvailableSpace;
		}

		//remove the food consumed from the creature so that it can try to not die ;)
		prey->food -= foodConsumed;
		//also, inflict a 10% loss on the prey after we've taken our share
		prey->food *= 0.9;

		food += foodConsumed;
	} else {
		int foodGain = getSize() * foodProductionRate;

		food += foodGain;
	}
}

void Organism::consumeFood(Statistics* simStats) {
	//scaled from 70 to 100% of foodconsumption, we have a chance to die
	//if below 70%, it's instant death
	float foodThreshold = foodConsumption * 0.5;
	float bonusChance = foodConsumption - foodThreshold;

	//std::cout<<food<<"|"<<foodConsumption<<std::endl;

	//lets play russian roulette for things like cancer or ebola
	if(rand() % 100 < suddenDeathChance) {
		dead = true;
		simStats->LogDeathStarved(symbol);
		return;
	}

	//just in case...
	if(bonusChance <= 0) {
		bonusChance = 1;
	}

	//std::cout<<food<<std::endl;
	food -= foodThreshold;
	if(food < 0) {
		dead = true;
		simStats->LogDeathStarved(symbol);
		return;
	}

	if(food > bonusChance) {
		food -= bonusChance;

		return;
	}

	//here's the chance-to-die roll
	//we can't have a random-death chance here (for now for easier stablization)
	if(rand() % 100 > (food/bonusChance) * 100) {
		dead = true;
		simStats->LogDeathMisfortune(symbol);
	}
}

void Organism::initialize(int ntoughness, int nagility, int nintelligence,
		int ntoughnessVariance, int nagilityVariance, int nintelligenceVariance) {
	dead = false;
	food = 0;
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

	//plants take penalty vs every organism, herbivores take penalty vs carnivores and omnivores
	//carnivores take advantage defensively vs other carnivores and omnivores
	//this will create a stronger advantage for archtypes of creatures
	//but only apply during rollCheck, as this is used for actual damage calculations

	float ourStat = getValue(stat, rollCheck);
	float theirStat = target->getValue(stat, rollCheck);

	return (ourStat > theirStat);
}


float Organism::getBabyFood() {//gets the value of how much baby food we have set aside of our own
	return breedThreshold * foodConsumption;
}

float Organism::takeBabyFood() {
	//return 0 if we simply can't fulfill this request
	if(food < getBabyFood()) {
		return 0;
	}

	food -= getBabyFood();
	return getBabyFood();
}

bool Organism::canBreed() {//checks if we have enough food to spare
	//arbitrary 80% of food is preserved beyond reproduction requirements
	if(food >= getBabyFood() + foodConsumption * 0.8) {
		return true;
	}

	return false;
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

	foodStatPenalty = parent1->foodStatPenalty;
	foodStatVariancePenalty = parent1->foodStatVariancePenalty;
	foodProductionRate = parent1->foodProductionRate;
	foodCapMultiplier = parent1->foodCapMultiplier;
	kills = 0;

	symbol = parent1->symbol;

	eatsPlants = parent1->eatsPlants;
	eatsAnimals = parent1->eatsAnimals;
	eatsFungus = parent1->eatsFungus;
	eatsDead = parent1->eatsDead;

	cannibal = parent1->cannibal;

	breedThreshold = parent1->breedThreshold;

	suddenDeathChance = parent1->suddenDeathChance;

	mutationRate = parent1->mutationRate;

	//take a minimum amount of food from each
		//pass it onto the baby in full
	food = parent1->takeBabyFood() + parent2->takeBabyFood();;
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

int Organism::getSize() {
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
	foodConsumption = (agility + intelligence + toughness)*foodStatPenalty +
			(agilityVariance + intelligenceVariance + toughnessVariance)*foodStatVariancePenalty;


	//base foodcap on consumption
	foodCap = foodConsumption*foodCapMultiplier;
}
