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
	void eat();
	void reproduce();

	void initialize(int toughness, int agility, int intelligence,
			int toughnessVariance, int agilityVariance, int intelligenceVariance);
	void initializeClass(ArchType archtype);
	void initializeRandom(int maxStatRange, int maxStatVarianceRange);

	float getValue(Stat stat, bool rollCheck = true);
	bool stronger(Organism* target, Stat stat, bool rollCheck = true);

	float getStatTotal();

	std::string name;
	ArchType archtype;


	float cachedFoodConsumption;
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
};

#endif /* ORGANISM_H_ */
