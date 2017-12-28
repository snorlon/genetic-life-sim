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

enum ArchType { plant, herbivore, omnivore, carnivore };
enum Stat { Toughness, Agility, Intelligence };

class Organism {
public:
	Organism();
	virtual ~Organism();
	void tickTurn();

	bool attack();
	void eat(Organism* prey = NULL);
	void reproduce();


	void initialize(int toughness, int agility, int intelligence,
			int toughnessVariance, int agilityVariance, int intelligenceVariance);
	void initializeClass(ArchType archtype);
	void initializeRandom(int maxStatRange, int maxStatVarianceRange);

	float getValue(Stat stat, bool rollCheck = true);
	bool stronger(Organism* target, Stat stat, bool rollCheck = true);

	void beBorn(Organism* parent1, Organism* parent2, int mutationRate);

	float getStatTotal();

	int getToughness();
	int getAgility();
	int getIntelligence();

	std::string name;
	ArchType archtype;


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

	float kills;

	bool dead;

protected:

	int toughness;
	int agility;
	int intelligence;

	int toughnessVariance;
	int agilityVariance;
	int intelligenceVariance;

	//returns a random value between two integers without relationship
	int randomStatInRange(int val1, int val2);
	int mutateStat(int value, int mutationRate);
};

#endif /* ORGANISM_H_ */
