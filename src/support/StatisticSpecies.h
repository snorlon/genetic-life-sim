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
	string speciesName;
	string speciesSymbol;
	int survivingPopulation;
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
