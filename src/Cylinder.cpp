/*
 * Cylinder.cpp
 *
 *  Created on: 20 May 2011
 *      Author: halukcy
 */

#include "Cylinder.h"

Cylinder::Cylinder() {
	frontVector = frontVectorInit = Vector3(0.0, 0.0, 1.0);
	rightVector = rightVectorInit = Vector3(1.0, 0.0, 0.0);
	upVector = upVectorInit = Vector3(0.0, -1.0, 0.0);

	posVector = posVectorInit = Vector3(0.0, 0.0, 0.0);
	rotationAngles = rotationAnglesInit = Vector3(0.0, 0.0, 0.0);

	qobj = gluNewQuadric();

	radius = 0.5f;
	height = 2.0f;
}

Cylinder::~Cylinder() {
	// TODO Auto-generated destructor stub
}

void Cylinder::Init(float h, float r) {
	height = h;
	radius = r;
}

//#define VECTORS
void Cylinder::Render() {
	NormalizeVectors();
#ifdef VECTORS
	glBegin(GL_LINES);
	//front
	glColor3f(0.8, 0.0, 0.0);
	glVertex3f(posVector[0], posVector[1], posVector[2]);
	glVertex3f(frontVector[0], frontVector[1], frontVector[2]);
	//up
	glColor3f(0.0, 0.8, 0.0);
	glVertex3f(posVector[0], posVector[1], posVector[2]);
	glVertex3f(upVector[0], upVector[1], upVector[2]);
	//right
	glColor3f(0.8, 0.8, 0.0);
	glVertex3f(posVector[0], posVector[1], posVector[2]);
	glVertex3f(rightVector[0], rightVector[1], rightVector[2]);
	glEnd();

#else

	glPushMatrix(); //1
	glTranslatef(-posVector[0], -posVector[1], posVector[2]);
	glPushMatrix(); //2
	glRotatef(rotationAngles[1], upVectorInit[0], upVectorInit[1], upVectorInit[2]);
	glPushMatrix(); //3
	glRotatef(rotationAngles[2], frontVectorInit[0], frontVectorInit[1], frontVectorInit[2]);
	glPushMatrix(); //4
	glRotatef(rotationAngles[0], rightVectorInit[0], rightVectorInit[1], rightVectorInit[2]);
	glPushMatrix(); //5
	glColor3f(0.8, 0.8, 0.2);
	gluCylinder(qobj, radius, radius, height, 20, 5);
	glTranslatef(0.0, 0.0f, -height);
	glColor3f(0.2, 0.8, 0.8);
	gluCylinder(qobj, radius, radius, height, 20, 5);
	glPopMatrix(); //5
	glPopMatrix(); //4
	glPopMatrix(); //3
	glPopMatrix(); //2
	glPopMatrix(); //1
#endif
}
