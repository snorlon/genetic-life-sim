/*
 * Statistics.cpp
 *
 *  Created on: Jan 3, 2018
 *      Author: Erin Brown
 */

#include "Statistics.h"

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

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

void Statistics::LogStart(string speciesName, string speciesSymbol) {
	recordedTickData.at(getCurrentIndex()).speciesData.resize(recordedTickData.at(getCurrentIndex()).speciesData.size()+1);
	StatisticSpecies* newSpecies = &(recordedTickData.at(getCurrentIndex()).speciesData.at(recordedTickData.at(getCurrentIndex()).speciesData.size()-1));
	newSpecies->speciesName = speciesName;
	newSpecies->speciesSymbol = speciesSymbol;
}

void Statistics::LogStartCount(string speciesSymbol) {
	StatisticTick* currentTickData = &(recordedTickData.at(getCurrentIndex()));
	for(unsigned int i=0; i<currentTickData->speciesData.size(); i++) {
		StatisticSpecies* currentSpecies = &(currentTickData->speciesData.at(i));

		if(currentSpecies->speciesSymbol.compare(speciesSymbol) == 0) {
			//increment our population counters if found
			currentSpecies->startingPopulation++;
			return;
		}
	}

	cout<<"Error in statistics!"<<endl;
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

void Statistics::SaveStatisticsToFile(string filedir) {

	if(recordedTickData.size() <= 0) {
		cout<<"Simulation too short for statistics."<<endl;
		return;
	}

	cout<<"Outputting statistic data, please wait!"<<endl;

	//output csv data for each tick of the simulation
	ofstream mainOutput;
	ofstream individualSpeciesOutputs[recordedTickData.at(0).speciesData.size()];

	mainOutput.open((filedir+"tick.csv").c_str(),fstream::out | fstream::trunc);


	bool allFilesOpened = mainOutput.is_open();

	for(unsigned int i=0; i<recordedTickData.at(0).speciesData.size(); i++) {
		individualSpeciesOutputs[i].open((filedir+recordedTickData.at(0).speciesData.at(i).speciesName+".csv").c_str(),fstream::out | fstream::trunc);
		allFilesOpened = allFilesOpened and individualSpeciesOutputs[i].is_open();
		if(!individualSpeciesOutputs[i].is_open()) {
			cout<<"Failure on file "<<i<<" "<<(filedir+recordedTickData.at(0).speciesData.at(i).speciesName+".csv")<<endl;
		}
	}

	if(allFilesOpened) {
		//put labels into the csv
		mainOutput << "Tick";

		for(unsigned int i=0; i<recordedTickData[0].speciesData.size(); i++) {
			mainOutput << ",Name,Initial Population,Ending Population,Dead,Born,Kills,Failed Kills,Number Starved,Bad Luck Deaths,Overpopulation Deaths";
			individualSpeciesOutputs[i] << "Tick,Name,Initial Population,Ending Population,Dead,Born,Kills,Failed Kills,Number Starved,Bad Luck Deaths,Overpopulation Deaths\n";
		}

		mainOutput<<"\n";

		//logs tick and full stats in main output
		for(unsigned int i=0; i<recordedTickData.size(); i++) {
			mainOutput << recordedTickData.at(i).tickNumber;
			//logs tick and just species stats in species output
			//each species data element is its own column, each tick is its own row
			for(unsigned int j=0; j<recordedTickData[0].speciesData.size(); j++) {
				StatisticSpecies* data = &(recordedTickData[i].speciesData.at(j));
				individualSpeciesOutputs[j] << recordedTickData.at(i).tickNumber;
				mainOutput<<","<<data->speciesName<<","<<data->startingPopulation<<","<<data->survivingPopulation;
				mainOutput<<","<<data->newlyDeadPopulation<<","<<data->newlyBornPopulation<<","<<data->kills;
				mainOutput<<","<<data->failedKills<<","<<data->numDiedToStarvation<<","<<data->numDiedToChance;
				mainOutput<<","<<data->numDiedToOverpopulation;

				individualSpeciesOutputs[j]<<","<<data->speciesName<<","<<data->startingPopulation<<","<<data->survivingPopulation;
				individualSpeciesOutputs[j]<<","<<data->newlyDeadPopulation<<","<<data->newlyBornPopulation<<","<<data->kills;
				individualSpeciesOutputs[j]<<","<<data->failedKills<<","<<data->numDiedToStarvation<<","<<data->numDiedToChance;
				individualSpeciesOutputs[j]<<","<<data->numDiedToOverpopulation<<"\n";
			}

			mainOutput<<"\n";
		}
	}
	else {
		cout<<"Failed to open a file!"<<endl;
	}

	mainOutput.close();

	for(unsigned int i=0; i<recordedTickData.at(0).speciesData.size(); i++) {
		individualSpeciesOutputs[i].close();
	}

	cout<<"Done outputting statistical data to '"+filedir+"'"<<endl;
}
