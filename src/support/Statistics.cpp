/*
 * Statistics.cpp
 *
 *  Created on: Jan 3, 2018
 *      Author: Erin Brown
 */

#include "Statistics.h"

Statistics::Statistics() {
}

Statistics::~Statistics() {
	recordedTickData.clear();
}

void Statistics::LogTick(int tickNum) {
	//add a new element to the vector for this tick
	//assumes prior tick is finished
	recordedTickData.resize(recordedTickData.size()+1);
	recordedTickData.at(getCurrentIndex()).tickNumber = tickNum;
}

void Statistics::LogStartCount(string speciesName, string speciesSymbol) {
	StatisticTick* currentTickData = &(recordedTickData.at(getCurrentIndex()));
	for(unsigned int i=0; i<currentTickData->speciesData.size(); i++) {
		StatisticSpecies* currentSpecies = &(currentTickData->speciesData.at(i));

		if(currentSpecies->speciesSymbol.compare(speciesSymbol) == 0) {
			//increment our population counters if found
			currentSpecies->startingPopulation++;
			return;
		}
	}

	//otherwise, add new species to our stats for the tick
		//alternatively increment our counters here
	recordedTickData.at(getCurrentIndex()).speciesData.resize(recordedTickData.at(getCurrentIndex()).speciesData.size()+1);
	StatisticSpecies* newSpecies = &(recordedTickData.at(getCurrentIndex()).speciesData.at(recordedTickData.at(getCurrentIndex()).speciesData.size()-1));
	newSpecies->speciesName = speciesName;
	newSpecies->speciesSymbol = speciesSymbol;
	newSpecies->startingPopulation++;
}

void Statistics::LogSuccessfulHunt(string attackingSpeciesSymbol, string defendingSpeciesSymbol) {
	StatisticTick* currentTickData = &(recordedTickData.at(getCurrentIndex()));
	for(unsigned int i=0; i<currentTickData->speciesData.size(); i++) {
		StatisticSpecies* currentSpecies = &(currentTickData->speciesData.at(i));

		if(currentSpecies->speciesSymbol.compare(attackingSpeciesSymbol) == 0) {
			//increment our counters if found
			currentSpecies->kills++;
		}
	}
}

void Statistics::LogFailedHunt(string attackingSpeciesSymbol, string defendingSpeciesSymbol) {
	StatisticTick* currentTickData = &(recordedTickData.at(getCurrentIndex()));
	for(unsigned int i=0; i<currentTickData->speciesData.size(); i++) {
		StatisticSpecies* currentSpecies = &(currentTickData->speciesData.at(i));

		if(currentSpecies->speciesSymbol.compare(attackingSpeciesSymbol) == 0) {
			//increment our counters if found
			currentSpecies->failedKills++;
		}
		else if(currentSpecies->speciesSymbol.compare(defendingSpeciesSymbol) == 0) {
			//increment our counters if found
			currentSpecies->survivalSuccesses++;
		}
	}
}

void Statistics::LogDeathStarved(string speciesSymbol) {
	StatisticTick* currentTickData = &(recordedTickData.at(getCurrentIndex()));
	for(unsigned int i=0; i<currentTickData->speciesData.size(); i++) {
		StatisticSpecies* currentSpecies = &(currentTickData->speciesData.at(i));

		if(currentSpecies->speciesSymbol.compare(speciesSymbol) == 0) {
			//increment our counters if found
			currentSpecies->numDiedToStarvation++;
			currentSpecies->newlyDeadPopulation++;
			return;
		}
	}
}

void Statistics::LogDeathMisfortune(string speciesSymbol) {
	StatisticTick* currentTickData = &(recordedTickData.at(getCurrentIndex()));
	for(unsigned int i=0; i<currentTickData->speciesData.size(); i++) {
		StatisticSpecies* currentSpecies = &(currentTickData->speciesData.at(i));

		if(currentSpecies->speciesSymbol.compare(speciesSymbol) == 0) {
			//increment our counters if found
			currentSpecies->numDiedToChance++;
			currentSpecies->newlyDeadPopulation++;
			return;
		}
	}
}

void Statistics::LogDeathOverpopulation(string speciesSymbol) {
	StatisticTick* currentTickData = &(recordedTickData.at(getCurrentIndex()));
	for(unsigned int i=0; i<currentTickData->speciesData.size(); i++) {
		StatisticSpecies* currentSpecies = &(currentTickData->speciesData.at(i));

		if(currentSpecies->speciesSymbol.compare(speciesSymbol) == 0) {
			//increment our counters if found
			currentSpecies->numDiedToOverpopulation++;
			currentSpecies->newlyDeadPopulation++;
			return;
		}
	}
}

void Statistics::LogBirth(string speciesSymbol) {

	StatisticTick* currentTickData = &(recordedTickData.at(getCurrentIndex()));
	for(unsigned int i=0; i<currentTickData->speciesData.size(); i++) {
		StatisticSpecies* currentSpecies = &(currentTickData->speciesData.at(i));

		if(currentSpecies->speciesSymbol.compare(speciesSymbol) == 0) {
			//increment our counters if found
			currentSpecies->newlyBornPopulation++;
			return;
		}
	}
}

void Statistics::LogEndCount() {
	StatisticTick* currentTickData = &(recordedTickData.at(getCurrentIndex()));
	for(unsigned int i=0; i<currentTickData->speciesData.size(); i++) {
		StatisticSpecies* currentSpecies = &(currentTickData->speciesData.at(i));

		//total up our end numbers, should be accurate!
		currentSpecies->survivingPopulation = (currentSpecies->startingPopulation - currentSpecies->newlyDeadPopulation) + currentSpecies->newlyBornPopulation;
	}
}

int Statistics::getCurrentIndex() {
	return (recordedTickData.size()-1);
}

void SaveStatisticsToFile(string filedir = "files/output/stats/") {

}
