/*
 * PopulationManager.cpp
 *
 *  Created on: Nov 27, 2017
 *      Author: Erin Brown
 */

#include <iostream>
using namespace std;

#include "PopulationManager.h"

PopulationManager::PopulationManager(Config* nsimConfig, Parameters* nsimParams, Statistics* nsimStats, vector<Organism*> &templateOrganismTypes, int seed) {
	srand (seed);

	simConfig = nsimConfig;
	simParams = nsimParams;
	simStats = nsimStats;

	poolSize = 5;

	geneticPool = NULL;
	tick = 0;
	tickLimit = 0;
	geneticsInitialized = false;

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

	//prime the grouping of species independently of genetic pool array
	speciesGroups.clear();
	speciesGroups.resize(organismTemplates.size());

	//rebuild our indexes for species by symbols
	templateIndexes.clear();

	for(unsigned int i=0; i<organismTemplates.size(); i++) {
		templateIndexes[organismTemplates.at(i)->symbol] = i;
	}


	//generate our initial organism data
	geneticPool = new Organism[poolSize];
	for(unsigned int index=0; index<poolSize; index++) {
		Organism* rolledTemplate = pickRandomTemplate();

		geneticPool[index].instantiate(rolledTemplate);

	}

	geneticsInitialized = true;

	tickLimit = simParams->maxCycleCount;
}

void PopulationManager::run() {
	if(!geneticsInitialized) {
		cout<<"Genetics were not initialized!"<<endl;
		return;
	}

	while(tick < tickLimit || tickLimit == 0) {

		tickTurn();

		tick = tick + 1;

		if(tick % tickInfoFrequency == 0 || tick == 1) {
			//TODO: replace with a better way to handle this and strip out these counts
			cout<<"Tick " << tick <<": [ "<<countPlants()<<"p "<<countAnimals()<<"a "
					<<countMushrooms()<<"f ] Alive at end of tick" << endl;
		}
	}
	cout<<"Simulation has reached tick limit."<<endl;

	cout<<"Ending simulation with "<<countPlants()<<" plants."<<endl;
	cout<<"Ending simulation with "<<countAnimals()<<" animals."<<endl;
	cout<<"Ending simulation with "<<countMushrooms()<<" fungi."<<endl;

	end();
}

void PopulationManager::end() {
	//Output results here, such as population changes, organisms consumed, organisms killed, and genetic distribution across population
	simStats->SaveStatisticsToFile();

	delete[] geneticPool;
	geneticPool = NULL;
}

void PopulationManager::tickTurn() {
	//log statistics
	simStats->LogTick(tick);

	//clear prior cache data
	for(unsigned int i=0; i<organismTemplates.size(); i++) {
		speciesGroups.at(i).clear();
		simStats->LogStart(organismTemplates.at(i)->name, organismTemplates.at(i)->symbol);
	}

	creatureTurnOrder.clear();

	//populate caches for each species
	for(unsigned int i=0; i < poolSize; i++) {
		Organism* self = &geneticPool[i];

		if(!self->dead) {
			simStats->LogStartCount(self->symbol);
		}

		speciesGroups.at(templateIndexes[self->symbol]).push_back(self);
	}

	//build sorted caches by stat
	vector<vector<Organism*> > leastTough(speciesGroups);
	vector<vector<Organism*> > leastAgile(speciesGroups);
	vector<vector<Organism*> > leastSmart(speciesGroups);
	for(unsigned int i=0; i<speciesGroups.size(); i++) {
		sort(leastTough.at(i).begin(), leastTough.at(i).end(), sortByTough);
		sort(leastAgile.at(i).begin(), leastAgile.at(i).end(), sortByAgile);
		sort(leastSmart.at(i).begin(), leastSmart.at(i).end(), sortByIntelligence);
	}

	//sort turn order by stat total
	sort(creatureTurnOrder.begin(), creatureTurnOrder.end(), sortTurnOrder);

	//cache size of each creature group
	int plantCount = countPlants();
	int animalCount = countAnimals();
	int fungiCount = countMushrooms();

	//grant each creature an attack cycle
	for(unsigned int i=0; i < poolSize; i++) {
		Organism* self = &geneticPool[i];

		//for readability later
		bool eatPlants = self->eatsPlants;
		bool eatAnimals = self->eatsAnimals;
		bool eatFungi = self->eatsFungus;
		//bool eatDead = self->eatsDead;TODO: Unused
		bool eatOwnKind = self->cannibal;

		//the dead can't attack or eat...
		if(self->dead) {
			continue;
		}

		//build a vector of organisms we can consume, one-three picked at random from each species
			//less if we're more versatile, more if we are a picky eater
			//do not add duplicate units to vector!! treat it as failed rolls to not reroll
		vector<Organism*> prey;
		prey.clear();

		for(unsigned int i=0; i<organismTemplates.size(); i++) {
			ArchType archtype = organismTemplates.at(i)->archtype;

			//skip logic if there aren't any alive left, saving some computation...
			if(speciesGroups.at(i).size() <= 0) {
				continue;
			}

			//first do a cannibalism check!
			if(!eatOwnKind) {
				if(organismTemplates.at(i)->symbol.compare(self->symbol) == 0) {
					//skip species if it's our own kind!
					continue;
				}
			}

			//verify that we can eat them
			if((archtype == plant && eatPlants)||(archtype == animal && eatAnimals)||(archtype == fungus && eatFungi)) {
				//pick a prey to consume
				//cap at weakest 100 to pick from for now
				Organism* firstPick = getWeightedWeakest(speciesGroups.at(i), 100);

				//get second pick only if we have at least 1 group we won't eat
				Organism* secondPick = firstPick;
				if(!eatPlants || !eatAnimals || !eatFungi) {
					secondPick = getWeightedWeakest(speciesGroups.at(i), 100);
				}

				//get third pick only if we have at least 2 groups we won't eat
				Organism* thirdPick = firstPick;
				if((!eatPlants && !eatAnimals) || (!eatFungi && !eatAnimals) || (!eatPlants && !eatFungi)) {
					thirdPick = getWeightedWeakest(speciesGroups.at(i), 100);
				}

				//add them all to our list
				if(firstPick != NULL) {
					prey.push_back(firstPick);
				}
				if(secondPick != firstPick) {
					prey.push_back(secondPick);
				}
				if(thirdPick != firstPick) {
					prey.push_back(thirdPick);
				}
			}
		}

		//let naturally produced food occur first
		if(self->foodProductionRate > 0) {
			self->eat();
		}

		//attempt to eat up to N prey, with 1 less for each failure to beat prey
			//penalty is to emulate getting injured during the hunt, adding up to starvation if repeated
		//stop after food cap is reached
		int attemptCount = 7;//arbitrary vaue for now
		for(unsigned int i=0; i<prey.size() && attemptCount > 0 && self->food < self->foodCap; i++) {
			//abort if enemy size is more than twice our size, with no penalty to attempt count
			if(prey.at(i)->getSize() > self->getSize()*2) {
				continue;
			}

			//attempt to hunt the prey
			//use toughness if target is a plant
			//must win 2/3 rolls otherwise
			bool successfulToughness = self->stronger(prey.at(i), Toughness, true);
			bool successfulAgility = false;
			bool successfulIntelligence = true;

			if(prey.at(i)->archtype != fungus && prey.at(i)->archtype != plant) {
				successfulAgility = self->stronger(prey.at(i), Agility, true);
				successfulIntelligence = self->stronger(prey.at(i), Intelligence, true);
			}

			//consume only up to capacity for food
			if((successfulToughness && successfulAgility) ||
					(successfulAgility && successfulIntelligence) ||
					(successfulToughness && successfulIntelligence)) {
				self->eat(prey.at(i));
				simStats->LogSuccessfulHunt(self->symbol, prey.at(i)->symbol);
			} else {
				simStats->LogFailedHunt(self->symbol, prey.at(i)->symbol);
			}

			//if we've made it here, we've truly made an attempt to hunt our target
			attemptCount--;
		}


		//if we fail to eat enough, and we can consume corpses, attempt to consume corpses randomly
			//corpses have a third of their original stat total, to emulate failure to handle tougher meat
		//TODO

	}


	//cull off organisms based on their original share of the current "space" we have at start of turn
	//50% death rate at 100% share, 0% at 0% share
	float plantPercent = plantCount / poolSize;
	float animalPercent = animalCount / poolSize;
	float fungiPercent = fungiCount / poolSize;

	//kill any creature that can't pay the food tax that is life
	for(unsigned int i=0; i < poolSize; i++) {
		if(geneticPool[i].dead) {
			continue;
		}

		geneticPool[i].consumeFood(simStats);

		//cull "space" strained species here
		if(!geneticPool[i].dead) {
			float usedPercent = plantPercent;

			if(geneticPool[i].archtype == animal) {
				usedPercent = animalPercent;
			} else if(geneticPool[i].archtype == fungus) {
				usedPercent = fungiPercent;
			}

			if(rand() % 100 < usedPercent * 90) {
				geneticPool[i].dead = true;
				simStats->LogDeathOverpopulation(geneticPool[i].symbol);
			}
		}
	}

	//populate caches for each living species

	//cross breed them with one another to fill up remaining slots
	//mutations resulting can exceed original max/minimum parameters of the simulation

	//stores living creatures separated by species
	vector<vector<Organism*> > livingSpecies;
	livingSpecies.resize(templateIndexes.size());
	//a separate cache without species separation to better randomize breeding in space limitations
	vector<Organism*> breedingLivingSpecies;

	vector<Organism*> deadCreatures;
	deadCreatures.clear();

	for(unsigned int i=0; i<poolSize; i++) {
		Organism* self = &geneticPool[i];
		if(!self->dead) {
			livingSpecies.at(templateIndexes[self->symbol]).push_back(self);
			breedingLivingSpecies.push_back(self);
		} else {
			deadCreatures.push_back(self);
		}
	}

	//shuffle breeding pool
	random_shuffle ( breedingLivingSpecies.begin(), breedingLivingSpecies.end(), seededRandom);

	//survival numbers
	if(tick % tickInfoFrequency == 0 || tick == 1) {
		//TODO: replace with a better way to handle this and strip out these counts
		cout<<"[ "<<countPlants()<<"p "<<countAnimals()<<"a "
				<<countMushrooms()<<"f ] Survived" << endl;
	}

	//% of dead pool size to be added each tick to fill in gaps
	float percentNewRandom = this->simParams->randomPerBreedCycle;

	//up until this portion of the dead pool is reached, generate new units
	for(unsigned int i=0; i<deadCreatures.size()*percentNewRandom; i++) {
		Organism* rolledTemplate = pickRandomTemplate();

		deadCreatures.at(0)->instantiate(rolledTemplate);

		//make sure to remove the now-living from the dead list to prevent them being replaced
		deadCreatures.erase(deadCreatures.begin());
	}

	//output data
	int babiesMade = 0;

	//this algorithm assumes binary reproduction without specific sexes
	//under it, as long as ample resources are provided, a successful parent can reproduce many times per cycle
	//iterate across all living things
	for(unsigned int i=0; i<breedingLivingSpecies.size(); i++) {
		Organism* self = breedingLivingSpecies.at(i);
		int speciesID = -1;

		int breedAttemptsLeft = 5;//TODO make a parameter

		//can only breed if they have at least THRESHOLD excess food to survive
		if(!self->canBreed()) {
			continue;
		}

		for(unsigned int k=0; k<organismTemplates.size(); k++) {
			if(organismTemplates.at(k)->symbol.compare(self->symbol) == 0) {
				speciesID = k;
			}
		}

		if(speciesID < 0) {
			cout<<"Unknown species entered pool!"<<endl;
			continue;
		}


		while(breedAttemptsLeft > 0 && deadCreatures.size() > 0) {
			//give them a chance to breed with a random from the same pool
				//reroll breeding target 5 times
			Organism* partner = NULL;

			partner = livingSpecies.at(speciesID).at(rand() % livingSpecies.at(speciesID).size());

			if(partner == self || !partner->canBreed()) {
				//this can not be themselves!
				breedAttemptsLeft--;
				continue;
			}

			//make the baby
			//baby takes a range of values between the parents two values
				//mutation can +- the result independently of this
			Organism* newBaby = deadCreatures.at(0);
			deadCreatures.erase(deadCreatures.begin());
			newBaby->beBorn(self, partner);
			babiesMade++;
			simStats->LogBirth(self->symbol);
		}
	}

	if(tick % tickInfoFrequency == 0 || tick == 1) {
		//cout<<"Made "<<babiesMade<<" children this turn."<<endl;
	}

	//finish up stat collection for the tick
	simStats->LogEndCount();
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

int PopulationManager::countAnimals() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(geneticPool[i].archtype == animal && !geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countMushrooms() {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(geneticPool[i].archtype == fungus && !geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countSpecies(string speciesName) {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(geneticPool[i].name.compare(speciesName)==0 && !geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

int PopulationManager::countNonSpecies(string speciesName) {
	int count = 0;

	for(unsigned int i=0; i<poolSize; i++) {
		if(geneticPool[i].name.compare(speciesName)!=0 && !geneticPool[i].dead) {
			count++;
		}
	}

	return count;
}

bool PopulationManager::sortByTough(Organism* i, Organism* j) {
	return (j->stronger(i, Toughness, false)||(j->dead && !i->dead));
}

bool PopulationManager::sortByAgile(Organism* i, Organism* j) {
	return (j->stronger(i, Agility, false)||(j->dead && !i->dead));
}

bool PopulationManager::sortByIntelligence(Organism* i, Organism* j) {
	return (j->stronger(i, Intelligence, false)||(j->dead && !i->dead));
}

bool PopulationManager::sortTurnOrder(Organism* i, Organism* j) {
	float total_i = i->getSize();
	float total_j = j->getSize();
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

Organism* PopulationManager::getWeightedWeakest(vector<Organism*> &possibleTargets, unsigned int maxWeakestPullable) {
	if(possibleTargets.size() <= 0) {
		return NULL;
	}

	if(maxWeakestPullable > possibleTargets.size()-1) {
		maxWeakestPullable = possibleTargets.size()-1;
	}

	unsigned int index = getWeightedIndex(maxWeakestPullable);

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
