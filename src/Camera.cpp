#include "Camera.h"
#include <stdio.h>

Camera::Camera(){
	frontVector=frontVectorInit=Vector3(0.0, 0.0, -5.0);
	rightVector=rightVectorInit=Vector3(1.0, 0.0, 0.0);
	upVector=upVectorInit=Vector3(0.0, 1.0, 0.0);

	posVector=posVectorInit=Vector3(0.0, 0.0, 0.0);
	rotationAngles=rotationAnglesInit=Vector3(0.0, 0.0, 0.0);

	chase=false;
}


void Camera::Init(){
	//where are we
	Vector3 newPos = posVector+frontVector;

	//init the opengl camera
	gluLookAt(posVector[0],posVector[1],posVector[2],
			newPos[0],newPos[1],newPos[2],
			upVector[0],upVector[1],upVector[2]);

	lookBehind = false;
}

void Camera::LookBehind()
{
	lookBehind = !lookBehind;
}

bool Camera::IsLookingBehind()
{
	return lookBehind;
}

void Camera::ChaseMode(Vector3 front, Vector3 right, Vector3 up, Vector3 pos, GLfloat angle){
	frontVector=front;
	rightVector=right;
	upVector=up;
	posVector=pos;
}

