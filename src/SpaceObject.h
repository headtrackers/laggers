/*
 * SpaceObject.h
 *
 *  Created on: 20 May 2011
 *      Author: halukcy
 */

#ifndef SPACEOBJECT_H_
#define SPACEOBJECT_H_

#include"MathTerms.h"

class SpaceObject {
public:
	SpaceObject();
	virtual ~SpaceObject();

	void Render();
	void Reset();

	void Move(Vector3);
	void MoveX(float);
	void MoveY(float);
	void MoveZ(float);
	void RotateX(GLfloat);
	void RotateY(GLfloat);
	void RotateZ(GLfloat);

	void NormalizeVectors();

	Vector3 GetPosition();
	void SetPosition(Vector3 pos);
	Vector3 GetDirection();

	void print();
protected:
	Vector3 upVector;
	Vector3 frontVector;
	Vector3 rightVector;
	Vector3 posVector;
	Vector3 rotationAngles;

	Vector3 upVectorInit;
	Vector3 frontVectorInit;
	Vector3 rightVectorInit;
	Vector3 posVectorInit;
	Vector3 rotationAnglesInit;
};

#endif /* SPACEOBJECT_H_ */
