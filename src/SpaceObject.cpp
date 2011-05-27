/*
 * SpaceObject.cpp
 *
 *  Created on: 20 May 2011
 *      Author: halukcy
 */

#include "SpaceObject.h"
#include <stdio.h>

SpaceObject::SpaceObject() {
	// TODO Auto-generated constructor stub
}

SpaceObject::~SpaceObject() {
	// TODO Auto-generated destructor stub
}

void Render(){
}

void SpaceObject::NormalizeVectors(){
	frontVector.normalize();
	upVector.normalize();
	rightVector.normalize();
}

void SpaceObject::Move(Vector3 newPos){
	posVector=posVector+newPos;
}

void SpaceObject::SetPosition(Vector3 pos){
	posVector=pos;
}

void SpaceObject::MoveX(float distance){
	posVector=posVector+rightVector.normalize()*distance;
}

void SpaceObject::MoveY(float distance){
	posVector=posVector+upVector.normalize()*distance;
}

void SpaceObject::MoveZ(float distance){
	posVector=posVector+frontVector.normalize()*distance;
}

void SpaceObject::RotateX(GLfloat angle){
	//update rotation angle for correct transformation
	GLfloat tempAngle=rotationAngles[0]+angle;
	if(tempAngle>=360) tempAngle-=360;
	else if(tempAngle<-360) tempAngle+=360;

	rotationAngles.setX(tempAngle);

	//frontVector.rotateX(angle);

	frontVector = frontVector.rotateArbitrary(angle, rightVector);
	frontVector.normalize();

	upVector = rightVector.crossProduct(frontVector);
	print();
}

void SpaceObject::RotateY(GLfloat angle){
	GLfloat tempAngle=rotationAngles[1]+angle;
	if(tempAngle>=360) tempAngle-=360;
	else if(tempAngle<-360) tempAngle+=360;

	rotationAngles.setY(tempAngle);

	//rightVector.rotateY(angle);
	rightVector = rightVector.rotateArbitrary(angle, upVector);
	rightVector.normalize();

	frontVector = upVector.crossProduct(rightVector);
	print();
}

void SpaceObject::RotateZ(GLfloat angle){
	GLfloat tempAngle=rotationAngles[2]+angle;
	if(tempAngle>=360) tempAngle-=360;
	else if(tempAngle<-360) tempAngle+=360;

	rotationAngles.setZ(tempAngle);

	//upVector.rotateZ(angle);
	upVector=upVector.rotateArbitrary(angle, frontVector);

	upVector.normalize();
	rightVector = frontVector.crossProduct(upVector);
	print();
}

Vector3 SpaceObject::GetPosition(){
	return posVector;
}

Vector3 SpaceObject::GetDirection(){
	return frontVector;
}

void SpaceObject::print(){

	printf("angles -- %f - %f - %f\n", rotationAngles[0], rotationAngles[1], rotationAngles[2]);
	printf("----------------------------------------------------\n\n");
}

void SpaceObject::Reset(){
	upVector = upVectorInit;
	frontVector = frontVectorInit;
	rightVector = rightVectorInit;
	posVector = posVectorInit;
	rotationAngles=rotationAnglesInit;
}

