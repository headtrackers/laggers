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
	void setRadius(float radius);
	void setColour(Vector3 col);
private:
	float inRadius;
	float outRadius;
	GLUquadric *qobj;
	Vector3 colour;
};

#endif /* DONUT_H_ */
