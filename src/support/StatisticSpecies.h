/*
 * StatisticSpecies.h
 *
 *  Created on: Jan 18, 2018
 *      Author: Erin Brown
 */

#ifndef SUPPORT_STATISTICSPECIES_H_
#define SUPPORT_STATISTICSPECIES_H_
#include <string>

using namespace std;

struct StatisticSpecies {
public:
	StatisticSpecies() : speciesName("Error"), speciesSymbol("X"), startingPopulation(0),
	survivingPopulation(0), newlyDeadPopulation(0), newlyBornPopulation(0), kills(0),
	failedKills(0), survivalSuccesses(0), numDiedToStarvation(0), numDiedToChance(0), numDiedToOverpopulation(0){}

	string speciesName;
	string speciesSymbol;
	int startingPopulation;
	int survivingPopulation;//the population at the end of a tick
	int newlyDeadPopulation;
	int newlyBornPopulation;
	int kills;
	int failedKills;
	int survivalSuccesses;
	int numDiedToStarvation;
	int numDiedToChance;
	int numDiedToOverpopulation;
};



#endif /* SUPPORT_STATISTICSPECIES_H_ */
