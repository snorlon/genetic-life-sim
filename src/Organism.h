/*
 * Organism.h
 *
 *  Created on: Nov 22, 2017
 *      Author: root
 */

#ifndef ORGANISM_H_
#define ORGANISM_H_

#include <string>
#include <stdlib.h>

enum ArchType { plant, herbivore, omnivore, carnivore, animalia, fungus };
enum Stat { Toughness, Agility, Intelligence };

//plant variables
//per stat point
const float baseProductionRate = 2.0;
//every stat point below this will grant us more food production
const int productionRateComparisonUpperbound = 100;
//rate that food slows, every X of this will half production gains
const int plantFoodGainDecay = 1000;

//herbivore variables
//amount of food gained % from eating
const float herbivoreConsumptionGain = 0.5;
//rate that food slows, every X of this will half production gains
const int herbivoreFoodGainDecay = 600;

//carnivore variables
//amount of food gained % from eating
const float carnivoreConsumptionGain = 0.5;
//rate that food slows, every X of this will half production gains
const int carnivoreFoodGainDecay = 500;

//omnivore variables
//amount of food gained % from eating
const float omnivoreConsumptionGain = 0.5;
//rate that food slows, every X of this will half production gains
const int omnivoreFoodGainDecay = 600;

class Organism {
public:
	Organism();
	virtual ~Organism();
	void tickTurn();

	bool attack();
	//takes in a prey if a nonplant, will consume prey if they are passed and take in their food
	void eat(Organism* prey = NULL);
	//will kill a creature if they have insufficient food left to survive
	void consumeFood();
	void reproduce();


	void initialize(int toughness, int agility, int intelligence,
			int toughnessVariance, int agilityVariance, int intelligenceVariance);
	void initializeClass(ArchType archtype);
	void initializeRandom(int maxStatRange, int maxStatVarianceRange);

	float getValue(Stat stat, bool rollCheck = true);
	bool stronger(Organism* target, Stat stat, bool rollCheck = true);

	void beBorn(Organism* parent1, Organism* parent2);

	float getStatTotal();

	int getToughness();
	int getAgility();
	int getIntelligence();

	void recalculateFood();

	std::string name;
	std::string symbol;
	ArchType archtype;

	bool eatsPlants;
	bool eatsAnimals;
	bool eatsFungus;
	bool eatsDead;

	bool cannibal;

	double suddenDeathChance;



	//food is gained on kills, 50% of targets food is gained
	//an animal will not consume more than 25% more than they need to survive and will "eject" it at the food check cycle
	//a plant will not die if they have enough food left after the hunting cycle, but an animal dies after preyed
	//lets say plants gain more with lower stats, but less as they gain more and more as they don't consume
		//every 1000 food gains, their rate of gain decreases
			//this will lead to small plants peaking at food amount quickly whilst dieing quickly, like a weed
			//whereas bigger, tougher plants have less to spare and as such aren't targetted as much
			//big plants will become the basis for plant survival whilst smaller ones will breed more(TODO)
	//food consumption should be based on the assumption that target eats 2 prey at their stat size
	float foodConsumption;
	float food;
	float foodCap;

	float kills;

	bool dead;

	//constants for type (for now)
	int mutationRate;

	double breedThreshold;

	double foodStatPenalty;//bonus food consumption multiplier for stat total
	double foodStatVariancePenalty;//bonus food consumption multiplier for variance total
	double foodProductionRate;//% of stat total produced per turn
	double foodCapMultiplier;//% of food consumption that can be stored in stomach

	//used only for creating organisms in this type
	int toughnessMin;
	int toughnessMax;
	int agilityMin;
	int agilityMax;
	int intelligenceMin;
	int intelligenceMax;
	int toughnessVarianceMin;
	int toughnessVarianceMax;
	int agilityVarianceMin;
	int agilityVarianceMax;
	int intelligenceVarianceMin;
	int intelligenceVarianceMax;

protected:

	int toughness;
	int agility;
	int intelligence;

	int toughnessVariance;
	int agilityVariance;
	int intelligenceVariance;

	//returns a random value between two integers without relationship
	int randomStatInRange(int val1, int val2);
	int mutateStat(int value);
};

#endif /* ORGANISM_H_ */
