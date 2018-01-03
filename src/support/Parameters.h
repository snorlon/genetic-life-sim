/*
 * Parameters.h
 *
 *  Created on: Jan 3, 2018
 *      Author: root
 */

#ifndef SUPPORT_PARAMETERS_H_
#define SUPPORT_PARAMETERS_H_

#include "../Organism.h"

class Parameters {
public:
	Parameters();
	virtual ~Parameters();

	float calculateFoodConsumption(Organism* self);
	float calculateFoodCapacity(Organism* self);

private:
	int spawnRates[];
	float initialRandomPercent;
	float randomPerBreedCycle;

	int populationCap;
	int maxCycleCount;

	float statFoodPenalty;
	float statVarianceFoodPenalty;
};

#endif /* SUPPORT_PARAMETERS_H_ */
