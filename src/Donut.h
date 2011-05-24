/*
 * Donut.h
 *
 *  Created on: May 22, 2011
 *      Author: halukcy
 */

#ifndef DONUT_H_
#define DONUT_H_

#include "SpaceObject.h"

class Donut: public SpaceObject {
public:
	Donut();
	virtual ~Donut();

	void Render();
private:
	float inRadius;
	float outRadius;
	GLUquadric *qobj;
};

#endif /* DONUT_H_ */
