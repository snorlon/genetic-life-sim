/*
 * Parameters.cpp
 *
 *  Created on: Jan 3, 2018
 *      Author: root
 */

#include "Parameters.h"

Parameters::Parameters() {
	statFoodPenalty = 0.01;
	statVarianceFoodPenalty = 0.01;

	initialRandomPercent = 0.1;
	randomPerBreedCycle = 0.1;

	populationCap = 10;

	maxCycleCount = 5;

}

Parameters::~Parameters() {
	// TODO Auto-generated destructor stub
}

float Parameters::getFoodPenalty() {
	return statFoodPenalty;
}

float Parameters::getVarianceFoodPenalty() {
	return statVarianceFoodPenalty;
}
