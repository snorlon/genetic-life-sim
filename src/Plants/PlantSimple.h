/*
 * PlantSimple.h
 *
 *  Created on: Nov 27, 2017
 *      Author: root
 */

#ifndef PLANTS_PLANTSIMPLE_H_
#define PLANTS_PLANTSIMPLE_H_

#include "../Organism.h"

class PlantSimple: public Organism {
public:
	PlantSimple();
	virtual ~PlantSimple();

	void tickTurn();

	bool attack();
	void eat();
	void reproduce();
protected:
	float foodRate;
	float foodVariance;
	float growthRate;
	float growthVariance;
};

#endif /* PLANTS_PLANTSIMPLE_H_ */
