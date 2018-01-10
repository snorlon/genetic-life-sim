/*
 * PopulationManager.cpp
 *
 *  Created on: Nov 27, 2017
 *      Author: root
 */

#include <iostream>
using namespace std;

#include "PopulationManager.h"

PopulationManager::PopulationManager(Config* nsimConfig, Parameters* nsimParams, vector<Organism*> &templateOrganismTypes, int seed) {
	srand (seed);

	simConfig = nsimConfig;
	simParams = nsimParams;

	poolSize = 5;

	geneticPool = NULL;
	tick = 0;
	tickLimit = 0;
	geneticsInitialized = false;
	evolutionInitialized = false;

	organismTemplates = templateOrganismTypes;

	cachedTotalProbability = 0;
}

PopulationManager::~PopulationManager() {
	// TODO Auto-generated destructor stub
}

void PopulationManager::initializeGenetics() {

	//set some variables for the simulation
	poolSize = simParams->populationCap;
	originalDistribution = simParams->spawnRates;


	//generate our initial organism data
	geneticPool = new Organism[poolSize];
	for(unsigned int index=0; index<poolSize; index++) {
		Organism* rolledTemplate = pickRandomTemplate();

		geneticPool[index].instantiate(rolledTemplate);

	}

	geneticsInitialized = true;

	tickLimit = simParams->maxCycleCount;

	evolutionInitialized = true;
}

void PopulationManager::run() {
	if(!geneticsInitialized) {
		cout<<"Genetics were not initialized!"<<endl;
		return;
	}

	if(!evolutionInitialized) {
		cout<<"Evolution was not initialized!"<<endl;
		return;
	}

	while(tick < tickLimit || tickLimit == 0) {

		tickTurn();

		tick = tick + 1;

		if(tick % tickInfoFrequency == 0 || tick == 1) {
			cout<<"Tick " << tick <<": [ "<<countPlants()<<"p "<<countHerbivores()<<"h "
					<<countCarnivores()<<"c "<<countOmnivores()<<"o ] Alive at end of tick" << endl;
		}
	}
	cout<<"Simulation has reached tick limit."<<endl;

	cout<<"Ending simulation with "<<countPlants()<<" plants."<<endl;
	cout<<"Ending simulation with "<<countHerbivores()<<" herbivores."<<endl;
	cout<<"Ending simulation with "<<countCarnivores()<<" carnivores."<<endl;
	cout<<"Ending simulation with "<<countOmnivores()<<" omnivores."<<endl;

	end();

	//run test on weighted random system
	/*int testIndexCount = 100;
	int counts[testIndexCount];
	int testSize = 100000;
	for(int i=0; i<testIndexCount; i++) {
		counts[i] = 0;
	}
	for(int i=0; i<testSize; i++) {
		counts[getWeightedIndex(testIndexCount)]++;
	}
	for(int i=0; i<testIndexCount / 10; i++) {
		for(int j=0; j<10; j++) {
			cout<<(counts[i*10 + j])<<"|";
		}
		cout<<endl;
	}*/
}

void PopulationManager::end() {
	//Output results here, such as population changes, organisms consumed, organisms killed, and genetic distribution across population

	delete[] geneticPool;
	geneticPool = NULL;
}

void PopulationManager::tickTurn() {
	//clear prior cache data
	plantLeastTough.clear();
	creatureLeastTough.clear();
	creatureLeastAgile.clear();
	creatureLeastSmart.clear();

	creatureTurnOrder.clear();

	//populate caches for plants, creatures
	//plantLeastTough.push_back(X);
	for(unsigned int i=0; i < poolSize; i++) {
		//if any are dead, we shouldn't add them
		if(geneticPool[i].dead) {
			continue;
		}

		//push separately for plants and creatures
		if(geneticPool[i].archtype == plant) {
			//plants get pushed to 1 vector
			plantLeastTough.push_back(&geneticPool[i]);
		}
		else {
			//creatures get pushed to 3 vectors
			creatureLeastTough.push_back(&geneticPool[i]);
			creatureLeastAgile.push_back(&geneticPool[i]);
			creatureLeastSmart.push_back(&geneticPool[i]);
			creatureTurnOrder.push_back(&geneticPool[i]);
		}
	}

	//sort caches by stat
	sort(plantLeastTough.begin(), plantLeastTough.end(), sortByTough);
	sort(creatureLeastTough.begin(), creatureLeastTough.end(), sortByTough);
	sort(creatureLeastAgile.begin(), creatureLeastAgile.end(), sortByAgile);
	sort(creatureLeastSmart.begin(), creatureLeastSmart.end(), sortByIntelligence);

	//sort turn order by stat total
	sort(creatureTurnOrder.begin(), creatureTurnOrder.end(), sortTurnOrder);

	//cache size of each creature group
	int plantCount = countPlants();
	int herbivoreCount = countHerbivores();
	int carnivoreCount = countCarnivores();
	int omnivoreCount = countOmnivores();

	//grant each creature an attack cycle
	for(unsigned int i=0; i < poolSize; i++) {
		ArchType creatureArchtype = geneticPool[i].archtype;
		Organism* self = &geneticPool[i];

		//the dead can't attack or eat...
		if(self->dead) {
			continue;
		}

		Organism* rolledTarget = NULL;
		Organism* rolledTarget2 = NULL;
		Organism* rolledTarget3 = NULL;
		unsigned int rolledIndex = 0;
		unsigned int rolledIndex1 = 0;
		unsigned int rolledIndex2 = 0;
		unsigned int rolledIndex3 = 0;

		vector<Organism*>* chosenGroup = NULL;

		//this tracks if we live or die
		bool fed = false;
		unsigned int pullable = 5;

		switch(creatureArchtype) {
		case plant:
			//plants can't attack :) they live by outreproducing mammals
			//to survive is to not be killed for them, we'll assume for now
			self->eat();

			break;
		case herbivore:
			//picks a random plant out of all plants, takes food from plant if possible
			//ensure there's at least two plants first..
			if(plantLeastTough.size() > 0) {
				//attempt multiple hunts if we fail the first time
				for(int j=0; j<5 && !fed && plantLeastTough.size() > 0; j++) {
					rolledIndex = rand() % plantLeastTough.size();
					rolledTarget = plantLeastTough.at(rolledIndex);

					bool successfulHunting = self->stronger(rolledTarget, Toughness, true);

					//target dies if pass
					if(successfulHunting) {
						self->eat(rolledTarget);
						fed = self->food >= self->foodCap;
					}
				}

			}
			break;
		case carnivore:
			//grabs weakest 20(PARAMETER?) of each stat, excluding self and those stronger than us in that stat
			pullable = 20;

			rolledTarget = getWeightedWeakest(creatureLeastTough, pullable, rolledIndex1);
			rolledTarget2 = getWeightedWeakest(creatureLeastAgile, pullable, rolledIndex2);
			rolledTarget3 = getWeightedWeakest(creatureLeastSmart, pullable, rolledIndex3);

			chosenGroup = &creatureLeastTough;

			//lives or dies on killing target
			if(creatureLeastTough.size() > 0) {
				//picks randomly from these on whom to attack
				rolledIndex = rand() % 3;

				switch(rolledIndex) {
				case 1:
					rolledIndex1 = rolledIndex2;
					rolledTarget = rolledTarget2;
					chosenGroup = &creatureLeastAgile;
					break;
				case 2:
					rolledIndex1 = rolledIndex3;
					rolledTarget = rolledTarget3;
					chosenGroup = &creatureLeastSmart;
					break;
				default:
					break;
				}

				if(rolledTarget != NULL) {
					bool successfulHunting1 = self->stronger(rolledTarget, Toughness, true);
					bool successfulHunting2 = self->stronger(rolledTarget, Agility, true);
					bool successfulHunting3 = self->stronger(rolledTarget, Intelligence, true);

					bool pairA = successfulHunting1 && successfulHunting2;
					bool pairB = successfulHunting1 && successfulHunting3;
					bool pairC = successfulHunting3 && successfulHunting2;

					//target dies if pass
					if(pairA || pairB || pairC) {
						self->eat(rolledTarget);
						fed = self->food >= self->foodCap;
					}
				}

			}
			break;
		case omnivore:
			//attempts hunting weakest 5 creatures, and 3 plants
			if(plantLeastTough.size() > 1) {
				//attempt multiple rolls if we fail the first time
				for(int j=0; j<3 && !fed; j++) {
					rolledIndex = rand() % plantLeastTough.size();
					rolledTarget = plantLeastTough.at(rolledIndex);

					bool successfulHunting = self->stronger(rolledTarget, Toughness, true);

					//target dies if pass
					if(successfulHunting) {
						fed = true;
						rolledTarget->dead = true;
					}
				}

			}

			//gets to attempt both, plant first, animal if plant fails
			pullable = 5;

			rolledTarget = getWeightedWeakest(creatureLeastTough, pullable, rolledIndex1);
			rolledTarget2 = getWeightedWeakest(creatureLeastAgile, pullable, rolledIndex2);
			rolledTarget3 = getWeightedWeakest(creatureLeastSmart, pullable, rolledIndex3);

			chosenGroup = &creatureLeastTough;

			//lives or dies on killing target
			if(creatureLeastTough.size() > 0 && !fed) {
				//picks randomly from these on whom to attack
				rolledIndex = rand() % 3;

				switch(rolledIndex) {
				case 1:
					rolledIndex1 = rolledIndex2;
					rolledTarget = rolledTarget2;
					chosenGroup = &creatureLeastAgile;
					break;
				case 2:
					rolledIndex1 = rolledIndex3;
					rolledTarget = rolledTarget3;
					chosenGroup = &creatureLeastSmart;
					break;
				default:
					break;
				}

				if(rolledTarget != NULL) {
					bool successfulHunting1 = self->stronger(rolledTarget, Toughness, true);
					bool successfulHunting2 = self->stronger(rolledTarget, Agility, true);
					bool successfulHunting3 = self->stronger(rolledTarget, Intelligence, true);

					bool pairA = successfulHunting1 && successfulHunting2;
					bool pairB = successfulHunting1 && successfulHunting3;
					bool pairC = successfulHunting3 && successfulHunting2;

					//target dies if pass
					if(pairA || pairB || pairC) {
						self->eat(rolledTarget);
						fed = self->food >= self->foodCap;
					}
				}

			}
			break;
		}


		//kill any creature that can't pay the food tax that is life
		for(unsigned int i=0; i < poolSize; i++) {
			if(geneticPool[i].dead) {
				continue;
			}

			geneticPool[i].consumeFood();
		}



		//add a bonus death penalty depending on population size compared to total population
		int popSize = plantCount;
		switch(self->archtype) {
		case herbivore:
			popSize = herbivoreCount;
			break;
		case carnivore:
			popSize = carnivoreCount;
			break;
		case omnivore:
			popSize = omnivoreCount;
			break;
		default:
			break;
		}

		//factors in for "hidden" factors that large populations create and helps mitigate any given population exploding
			//things like algae blooms as a result of too much excrement in a river, for example
		//(#/total #) * 0.4 chance of instantdeath
		if(popSize * 0.1 > rand() % poolSize) {
			//died to overpopulation results
			self->dead = true;
		}
	}


	//populate caches for still living plants, herbivore, omnivore, carnivore
	vector<Organism*> livingPlants;
	vector<Organism*> livingHerbivores;
	vector<Organism*> livingCarnivores;
	vector<Organism*> livingOmnivores;
	vector<Organism*> liveCreatures;

	//store vector of location for "dead" locations
	vector<Organism*> deadCreatures;
	deadCreatures.clear();

	for(unsigned int i=0; i < poolSize; i++) {
		ArchType creatureArchtype = geneticPool[i].archtype;
		Organism* self = &geneticPool[i];

		if(self->dead == true) {
			deadCreatures.push_back(self);
		} else {
			liveCreatures.push_back(self);
			switch(creatureArchtype) {
			case plant:
				livingPlants.push_back(self);
				break;
			case herbivore:
				livingHerbivores.push_back(self);
				break;
			case carnivore:
				livingCarnivores.push_back(self);
				break;
			case omnivore:
				livingOmnivores.push_back(self);
				break;
			}
		}
	}

	//at intervals, report how many of each type survived
	if(tick % tickInfoFrequency == 0) {
		int survivorCount[] = {0,0,0,0};

		for(unsigned int i=0; i<liveCreatures.size(); i++) {
			survivorCount[liveCreatures.at(i)->archtype]++;
		}

		cout<<"Survivors: { "<<survivorCount[plant]<<"p "<<survivorCount[herbivore]<<"h "
				<<survivorCount[carnivore]<<"c "<<survivorCount[omnivore]<<"o }"<<endl;
	}

	float percentNewRandom = 0.1;

	//safety check for no living things, we had a genocide on our hands!
	if(liveCreatures.size() <= 0) {
		if(tick % tickInfoFrequency == 0) {
			//cout<<"Everyone died!"<<endl;
		}
		percentNewRandom = 1;
	}

	//breed based on distribution with random from each pool, plus minor mutation shifts

	//minimum of 1% chance for an archetype to surface randomly to prevent full extinction of them
		//handled by scaling 100% to 96%
	const int baseRate = 1;
	int scaledSize = 100 - baseRate*4;

	int plantRate = baseRate;
	int herbivoreRate = baseRate;
	int carnivoreRate = baseRate;
	int omnivoreRate = baseRate;

	plantCount = livingPlants.size();
	herbivoreCount = livingHerbivores.size();
	carnivoreCount = livingCarnivores.size();
	omnivoreCount = livingOmnivores.size();
	int total = plantCount + herbivoreCount + carnivoreCount + omnivoreCount;

	//in case we have no survivors
	if(total <= 0) {
		total = 1;
	}

	plantRate += (plantCount / total) * scaledSize;
	herbivoreRate += (herbivoreCount / total) * scaledSize;
	carnivoreRate += (carnivoreCount / total) * scaledSize;
	omnivoreRate += (omnivoreCount / total) * scaledSize;

	total = plantRate + herbivoreRate + carnivoreRate + omnivoreRate;

	for(unsigned int i=0; i<deadCreatures.size(); i++) {
		Organism* self = deadCreatures.at(i);

		//chance to skip over corpse, to allow population totals to fluctuate a bit
		if(rand() % 100 < chanceToFailToBreed) {
			continue;
		}

		//have a portion be random new species
		if(i < deadCreatures.size() * percentNewRandom) {
			ArchType archtype = plant;
			int roll = rand() % total;
			if(roll <= plantRate) {
				archtype = plant;
			} else {
				roll -= plantRate;
				if(roll <= herbivoreRate) {
					archtype = herbivore;
				} else {
					roll -= herbivoreRate;
					if(roll <= carnivoreRate) {
						archtype = carnivore;
					} else {
						roll -= carnivoreRate;
						if(roll <= omnivoreRate) {
							archtype = omnivore;
						} else {
							cout<<"CRITICAL ERROR in random breeding cycle!"<<endl;
						}
					}
				}
			}

			self->initializeClass(archtype);
			self->initializeRandom(70, 30);
		} else {
			//roll a 100% random creature
			Organism* parent1 = liveCreatures.at(rand() % liveCreatures.size());

			//roll a creature in a compatible group to breed with first creature
			Organism* parent2 = NULL;
			switch(parent1->archtype) {
			case plant:
				parent2 = livingPlants.at(rand() % livingPlants.size());
				break;
			case herbivore:
				parent2 = livingHerbivores.at(rand() % livingHerbivores.size());
				break;
			case carnivore:
				parent2 = livingCarnivores.at(rand() % livingCarnivores.size());
				break;
			case omnivore:
				parent2 = livingOmnivores.at(rand() % livingOmnivores.size());
				break;
			}

			if(parent2 != NULL) {
				//update corpses' data to be a mix of parents with mutation
				self->beBorn(parent1, parent2);
			}
		}

	}
}



int PopulationManager::countPlants() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(geneticPool[i].archtype == plant && !geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countHerbivores() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(geneticPool[i].archtype == herbivore && !geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countCarnivores() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(geneticPool[i].archtype == carnivore && !geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countOmnivores() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(geneticPool[i].archtype == omnivore && !geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countAnimals() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(geneticPool[i].archtype != plant && !geneticPool[i].dead) {
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

//Currently 30% chance of taking a given
int PopulationManager::getWeightedIndex(unsigned int maxIndex, unsigned int minIndex) {
	int threshold = 30;

	//iterate across, reducing till we get our roll
	for(unsigned int i=minIndex; i<=maxIndex; i++) {
		if(rand() % 100 < threshold) {
			return i;
		}
	}

	//by default, return the last possible option as a safeguard
	return maxIndex;
}

Organism* PopulationManager::getWeightedWeakest(vector<Organism*> &possibleTargets, unsigned int maxWeakestPullable, unsigned int &index) {
	if(maxWeakestPullable > possibleTargets.size()) {
		maxWeakestPullable = possibleTargets.size();
	}

	index = getWeightedIndex(maxWeakestPullable);

	return possibleTargets.at(index);
}


Organism* PopulationManager::pickRandomTemplate() {
	//recalculate cached probability total if not set yet
	if(cachedTotalProbability == 0) {
		for(unsigned int i=0; i<originalDistribution.size() && i<organismTemplates.size(); i++) {
			cachedTotalProbability += originalDistribution.at(i);
		}
	}

	int rolledProbability = rand() % cachedTotalProbability;

	//pick a weighted choice among our templates to use
	for(unsigned int i=0; i<originalDistribution.size() && i<organismTemplates.size(); i++) {
		rolledProbability -= originalDistribution.at(i);

		if(rolledProbability < 0) {
			return organismTemplates.at(i);
		}
	}

	return organismTemplates.at(organismTemplates.size() - 1);
}
