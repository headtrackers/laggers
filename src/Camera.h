#ifndef __CAMERA
#define __CAMERA


#include "SpaceObject.h"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Camera : public SpaceObject{
private:
	bool lookBehind;

public:
	Camera();

	bool chase;

	void Init();
	void ChaseMode(Vector3 front, Vector3 right, Vector3 up, Vector3 pos, GLfloat angle);
	void OverViewMode(Vector3 front, Vector3 right, Vector3 up, Vector3 pos);

	void LookBehind();
	bool IsLookingBehind();

};

#endif
