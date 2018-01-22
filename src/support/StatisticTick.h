/*
 * StatisticTick.h
 *
 *  Created on: Jan 18, 2018
 *      Author: Erin Brown
 */

#ifndef SUPPORT_STATISTICTICK_H_
#define SUPPORT_STATISTICTICK_H_
#include "StatisticSpecies.h"
#include <vector>


struct StatisticTick {
public:
	StatisticTick() : tickNumber(0){}

	int tickNumber;
	vector<StatisticSpecies> speciesData;
	/* Holds:
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
	 */
};


#endif /* SUPPORT_STATISTICTICK_H_ */
