#ifndef __MATHTERM
#define __MATHTERM
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#define PI 3.141592653589793238462643383279
#define TWOPI 2*PI

typedef GLfloat Matrix[16];
class Vector4;

struct Point3{
	GLfloat x;
	GLfloat y;
	GLfloat z;

	Point3(float X, float Y, float Z) : x(X), y(Y), z(Z){}
};

struct Plane{
	GLfloat A,B,C,D;
};

class Vector3{
protected:
	GLfloat x;
	GLfloat y;
	GLfloat z;

public:
	Vector3();
	Vector3(GLfloat xpos, GLfloat ypos, GLfloat zpos)
		:x(xpos), y(ypos), z(zpos)
		{}

	bool isZero();
	void setX(GLfloat);
	void setY(GLfloat);
	void setZ(GLfloat);

	inline GLfloat length();
	Vector3 normalize();

	void rotateX(GLfloat);
	void rotateY(GLfloat);
	void rotateZ(GLfloat);
	GLfloat angleBetween(Vector3 rhs);
	Vector3 rotateArbitrary(GLfloat angle, Vector3 vec);


	Vector3 crossProduct(Vector3);
	Vector3 operator=(Vector3);
	Vector3 operator+ (Vector3);
	Vector3 operator- (Vector3);
	GLfloat operator* (Vector3);
	Vector3 operator* (GLfloat);
	Vector3 operator/ (GLfloat);
	float operator[](int in);
};

class Vector4 : public Vector3 {
private:
	GLfloat w;
public:
	Vector4();
	Vector4(GLfloat xpos, GLfloat ypos, GLfloat zpos, GLfloat wpos);

	inline GLfloat W();
	void setW(GLfloat);

	Vector4 operator=(Vector4);
	Vector4 operator=(Vector3);
	GLfloat operator*(Vector4);
	GLfloat operator*(Vector3);

	friend class Vector3;
};



#endif
