/*
 * Path.cpp
 *
 *  Created on: May 24, 2011
 *      Author: halukcy
 */

#include "Path.h"

Path::Path() {
	width=10;
}

Path::~Path() {
	// TODO Auto-generated destructor stub
}

void Path::AddPoint(Point3 pt) {
	points.push_back(pt);
}

void Path::Render() {
	int size = points.size();
	glPushMatrix(); //1
	glTranslatef(-posVector[0], -posVector[1], posVector[2]);
	glPushMatrix(); //2
	glRotatef(rotationAngles[1], upVectorInit[0], upVectorInit[1], upVectorInit[2]);
	glPushMatrix(); //3
	glRotatef(rotationAngles[2], frontVectorInit[0], frontVectorInit[1], frontVectorInit[2]);
	glPushMatrix(); //4
	glRotatef(rotationAngles[0], rightVectorInit[0], rightVectorInit[1], rightVectorInit[2]);
	glPushMatrix(); //5
	glColor3f(0.8, 0.2, 0.2);
	//
	glLineWidth(width);
	glBegin(GL_LINE_STRIP);
	for(int i =0 ; i< size; i++){
		glVertex3f(points[i].x, points[i].y, points[i].z);
	}
	glEnd();
	//
	glPopMatrix(); //5
	glPopMatrix(); //4
	glPopMatrix(); //3
	glPopMatrix(); //2
	glPopMatrix(); //1
}
