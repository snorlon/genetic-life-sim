/*
 * Statistics.h
 *
 *  Created on: Jan 3, 2018
 *      Author: Erin Brown
 */

#ifndef SUPPORT_STATISTICS_H_
#define SUPPORT_STATISTICS_H_

#include "StatisticTick.h"
#include "../Organism.h"

/*	Statistics info
 *
 *	Mid-run
 *		Extinction events (entire population die-outs)
 *
 *	End-of-run
 *		Total ticks run
 *		Total babies
 *		Total deaths
 *		Parameters
 *
 *	Population statistics post-run
 *		Hunting success rate per-population
 *		Peak population numbers and their ticks
 *		Survival %
 *		Total babies made per population
 *		Total deaths per population
 *
 *
 *	Mid-run excel data
 *		CSV format
 *
 *		Export a combined with each data member as a column
 *		Also, export a per-species csv file with each data member as a column
 *
 *		Tick
 *		Population numbers w/ total
 *		Population %s
 *		Population birth numbers
 *		Population death numbers
 */

class Statistics {
public:
	Statistics();
	virtual ~Statistics();

	//Controls statistic data collection and storage
	void LogTick(int tickNum, Organism* &geneticPool);
	void LogStartCount(string speciesName, string speciesSymbol);
	void LogSuccessfulHunt(string attackingSpeciesName, string defendingSpeciesName);
	void LogFailedHunt(string attackingSpeciesName, string defendingSpeciesName);
	void LogDeathStarved(string speciesSymbol);
	void LogDeathMisfortune(string speciesSymbol);
	void LogDeathOverpopulation(string speciesSymbol);
	void LogBirth(string speciesSymbol);
	void LogEndCount(string speciesSymbol);
	void EndTick();

private:
	vector<StatisticTick> recordedTickData;

	StatisticTick* currentTickData;
};

#endif /* SUPPORT_STATISTICS_H_ */
