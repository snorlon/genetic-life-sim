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

	this->percentRandom = 0;
	this->mutationRate = 0.1;
	this->poolSize = 5;
	this->weightedBreeding = false;

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

	int plantWeight = percentPlant*100;
	int herbivoreWeight = percentHerbivore*100;
	int carnivoreWeight = percentCarnivore*100;
	int omnivoreWeight = 100 - plantWeight - herbivoreWeight - carnivoreWeight;
	int randomWeight = percentRandom * 100;

	int totalWeight = plantWeight+herbivoreWeight+carnivoreWeight+omnivoreWeight+randomWeight;

	//generate our initial organism data
	this->geneticPool = new Organism[this->poolSize];
	for(unsigned int index=0; index<this->poolSize; index++) {
		//give them a balanced chance of each archtype
		int roll = rand() % totalWeight;

		roll -= plantWeight;
		if(roll < 0) {
			this->geneticPool[index].initializeClass(plant);
		} else {
			roll -= herbivoreWeight;
			if(roll < 0) {
				this->geneticPool[index].initializeClass(herbivore);
			} else {
				roll -= carnivoreWeight;
				if(roll < 0) {
					this->geneticPool[index].initializeClass(carnivore);
				} else {
					roll -= omnivoreWeight;
					if(roll < 0) {
						this->geneticPool[index].initializeClass(omnivore);
					} else {
						this->geneticPool[index].initializeClass((ArchType)(rand() % 4));
					}
				}
			}
		}

		//initialize them with a range of random values
		this->geneticPool[index].initializeRandom(70, 30);
	}

	this->originalCount[0] = this->countPlants();
	this->originalCount[1] = this->countHerbivores();
	this->originalCount[2] = this->countCarnivores();
	this->originalCount[3] = this->countOmnivores();

	//prime weighting array, upper bound of 100 for now, assuming that's the max to draw from

	for(unsigned int i=0; i<this->MAX_WEIGHTS; i++) {
		if(i==0) {
			this->weightedTotal[i] = i+1;
		} else {
			this->weightedTotal[i] = i + this->weightedTotal[i-1];
		}
	}

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

		if(this->tick % tickInfoFrequency == 0 || this->tick == 1) {
			cout<<"Tick " << this->tick <<": [ "<<this->countPlants()<<"p "<<this->countHerbivores()<<"h "
					<<this->countCarnivores()<<"c "<<this->countOmnivores()<<"o ] Alive at end of tick" << endl;
		}
	}
	cout<<"Simulation has reached tick limit."<<endl;

	cout<<"Ending simulation with "<<this->countPlants()<<" from "<<this->originalCount[0]<<" plants."<<endl;
	cout<<"Ending simulation with "<<this->countHerbivores()<<" from "<<this->originalCount[1]<<" herbivores."<<endl;
	cout<<"Ending simulation with "<<this->countCarnivores()<<" from "<<this->originalCount[2]<<" carnivores."<<endl;
	cout<<"Ending simulation with "<<this->countOmnivores()<<" from "<<this->originalCount[3]<<" omnivores."<<endl;

	this->end();

	//run test on weighted random system
	/*int testIndexCount = 100;
	int counts[testIndexCount];
	int testSize = 100000;
	for(int i=0; i<testIndexCount; i++) {
		counts[i] = 0;
	}
	for(int i=0; i<testSize; i++) {
		counts[this->getWeightedIndex(testIndexCount)]++;
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
		//if any are dead, we shouldn't add them
		if(this->geneticPool[i].dead) {
			continue;
		}

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

	//cache size of each creature group
	int plantCount = countPlants();
	int herbivoreCount = countHerbivores();
	int carnivoreCount = countCarnivores();
	int omnivoreCount = countOmnivores();

	//grant each creature an attack cycle
	for(unsigned int i=0; i < poolSize; i++) {
		ArchType creatureArchtype = this->geneticPool[i].archtype;
		Organism* self = &this->geneticPool[i];

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
			if(this->plantLeastTough.size() > 0) {
				//attempt multiple hunts if we fail the first time
				for(int j=0; j<5 && !fed && this->plantLeastTough.size() > 0; j++) {
					rolledIndex = rand() % this->plantLeastTough.size();
					rolledTarget = this->plantLeastTough.at(rolledIndex);

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

			rolledTarget = getWeightedWeakest(this->creatureLeastTough, pullable, rolledIndex1);
			rolledTarget2 = getWeightedWeakest(this->creatureLeastAgile, pullable, rolledIndex2);
			rolledTarget3 = getWeightedWeakest(this->creatureLeastSmart, pullable, rolledIndex3);

			chosenGroup = &this->creatureLeastTough;

			//lives or dies on killing target
			if(this->creatureLeastTough.size() > 0) {
				//picks randomly from these on whom to attack
				rolledIndex = rand() % 3;

				switch(rolledIndex) {
				case 1:
					rolledIndex1 = rolledIndex2;
					rolledTarget = rolledTarget2;
					chosenGroup = &this->creatureLeastAgile;
					break;
				case 2:
					rolledIndex1 = rolledIndex3;
					rolledTarget = rolledTarget3;
					chosenGroup = &this->creatureLeastSmart;
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
			if(this->plantLeastTough.size() > 1) {
				//attempt multiple rolls if we fail the first time
				for(int j=0; j<3 && !fed; j++) {
					rolledIndex = rand() % this->plantLeastTough.size();
					rolledTarget = this->plantLeastTough.at(rolledIndex);

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

			rolledTarget = getWeightedWeakest(this->creatureLeastTough, pullable, rolledIndex1);
			rolledTarget2 = getWeightedWeakest(this->creatureLeastAgile, pullable, rolledIndex2);
			rolledTarget3 = getWeightedWeakest(this->creatureLeastSmart, pullable, rolledIndex3);

			chosenGroup = &this->creatureLeastTough;

			//lives or dies on killing target
			if(this->creatureLeastTough.size() > 0 && !fed) {
				//picks randomly from these on whom to attack
				rolledIndex = rand() % 3;

				switch(rolledIndex) {
				case 1:
					rolledIndex1 = rolledIndex2;
					rolledTarget = rolledTarget2;
					chosenGroup = &this->creatureLeastAgile;
					break;
				case 2:
					rolledIndex1 = rolledIndex3;
					rolledTarget = rolledTarget3;
					chosenGroup = &this->creatureLeastSmart;
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
			if(this->geneticPool[i].dead) {
				continue;
			}

			this->geneticPool[i].consumeFood();
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
		ArchType creatureArchtype = this->geneticPool[i].archtype;
		Organism* self = &this->geneticPool[i];

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
				self->beBorn(parent1, parent2, 3);
			}
		}

	}
}



int PopulationManager::countPlants() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(this->geneticPool[i].archtype == plant && !this->geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countHerbivores() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(this->geneticPool[i].archtype == herbivore && !this->geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countCarnivores() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(this->geneticPool[i].archtype == carnivore && !this->geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countOmnivores() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(this->geneticPool[i].archtype == omnivore && !this->geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countAnimals() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(this->geneticPool[i].archtype != plant && !this->geneticPool[i].dead) {
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

//Function will take in a count of 1 to MAX_WEIGHTS and return an index in that range -1
int PopulationManager::getWeightedIndex(unsigned int possibleCount) {
	if(possibleCount > MAX_WEIGHTS) {
		possibleCount = MAX_WEIGHTS;
	}
	int total = weightedTotal[possibleCount-1];
	int roll = rand() % total;

	//iterate across, reducing till we get our roll
	for(unsigned int i=possibleCount-1; i>0; i--) {
		roll -= i;

		if(roll < 0) {
			return i;
		}
	}

	//by default!
	return 0;
}

Organism* PopulationManager::getWeightedWeakest(vector<Organism*> &possibleTargets, unsigned int maxWeakestPullable, unsigned int &index) {
	if(maxWeakestPullable > possibleTargets.size()) {
		maxWeakestPullable = possibleTargets.size();
	}

	index = this->getWeightedIndex(maxWeakestPullable);

	return possibleTargets.at(index);
}
