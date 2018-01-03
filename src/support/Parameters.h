/*
 * Parameters.h
 *
 *  Created on: Jan 3, 2018
 *      Author: root
 */

#ifndef SUPPORT_PARAMETERS_H_
#define SUPPORT_PARAMETERS_H_

#include "../Organism.h"
#include <vector>

using namespace std;

class Parameters {
public:
	Parameters();
	virtual ~Parameters();

	float getFoodPenalty();
	float getVarianceFoodPenalty();

	vector<float> spawnRates;
	float initialRandomPercent;
	float randomPerBreedCycle;

	int populationCap;
	int maxCycleCount;

	float statFoodPenalty;
	float statVarianceFoodPenalty;
};

#endif /* SUPPORT_PARAMETERS_H_ */
