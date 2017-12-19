/*
 * PlantSimple.cpp
 *
 *  Created on: Nov 27, 2017
 *      Author: root
 */

#include "PlantSimple.h"

PlantSimple::PlantSimple() {
	Organism();

	this->archtype = plant;

	this->foodRate = 0;
	this->foodVariance = 0;

	this->growthRate = 0;
	this->growthVariance = 0;

}

PlantSimple::~PlantSimple() {
	// TODO Auto-generated destructor stub
}

void PlantSimple::tickTurn() {
	Organism::tickTurn();
}

bool PlantSimple::attack() {
	//Attacking must fail because a plant cannot consciously harm anything
	return false;
}

void PlantSimple::eat() {
	//Todo: add variance
	this->food = this->food + this->foodRate;
}

void PlantSimple::reproduce() {
	//Todo: add variance
	//this->population = this->population * this->growthRate;
}
