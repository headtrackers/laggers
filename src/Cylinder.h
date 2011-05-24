/*
 * Cylinder.h
 *
 *  Created on: 20 May 2011
 *      Author: halukcy
 */

#ifndef CYLINDER_H_
#define CYLINDER_H_

#include "SpaceObject.h"

class Cylinder : public SpaceObject {
public:
	Cylinder();
	virtual ~Cylinder();

	void Render();
	void Init(float r, float h);
private:
	float radius;
	float height;
	GLUquadric *qobj;
};

#endif /* CYLINDER_H_ */
