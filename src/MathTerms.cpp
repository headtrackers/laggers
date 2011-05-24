#include<cmath>
#include"MathTerms.h"

Vector3::Vector3() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

void Vector3::setX(GLfloat val) {
	x = val;
}

void Vector3::setY(GLfloat val) {
	y = val;
}

void Vector3::setZ(GLfloat val) {
	z = val;
}

inline GLfloat Vector3::length() {
	return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

bool Vector3::isZero() {
	return (x == 0.0 && y == 0.0 && z == 0.0);
}

GLfloat Vector3::angleBetween(Vector3 rhs) {
	GLfloat dot = *this * rhs;
	return acosf(dot / (this->length() * rhs.length()));
}

Vector3 Vector3::normalize() {
	GLfloat len = this->length();

	this->x /= len;
	this->y /= len;
	this->z /= len;
	return *this;
}

Vector3 Vector3::operator =(Vector3 rhs) {
	this->x = rhs[0];
	this->y = rhs[1];
	this->z = rhs[2];
	return *this;
}

Vector3 Vector3::operator +(Vector3 rhs) {
	Vector3 result(this->x + rhs[0], this->y + rhs[1], this->z + rhs[2]);
	return result;
}

Vector3 Vector3::operator -(Vector3 rhs) {
	Vector3 result(this->x - rhs[0], this->y - rhs[1], this->z - rhs[2]);
	return result;
}

//dot product
GLfloat Vector3::operator *(Vector3 rhs) {
	return this->x * rhs[0] + this->y * rhs[1] + this->z * rhs[2];
}

//magnitude multiply
Vector3 Vector3::operator *(GLfloat rhs) {
	return Vector3(this->x * rhs, this->y * rhs, this->z * rhs);
}

//magnitude division
Vector3 Vector3::operator /(GLfloat rhs) {
	return Vector3(this->x / rhs, this->y / rhs, this->z / rhs);
}

float Vector3::operator [](int in) {
	switch (in) {
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	default:
		return 0.0f;
	}
}

//cross product
Vector3 Vector3::crossProduct(Vector3 rhs) {
	return Vector3(this->y * rhs[2] - this->z * rhs[1], this->z
			* rhs[0] - this->x * rhs[2], this->x * rhs[1] - this->y
			* rhs[0]);
}

void Vector3::rotateX(GLfloat angle) {
	if (angle == 0)
		return;
	this->y = cosf(angle * PI / 180) * this->y - sinf(angle * PI / 180)
			* this->z;
	this->z = sin(angle * PI / 180) * this->y + cosf(angle * PI / 180)
			* this->z;
}

void Vector3::rotateY(GLfloat angle) {
	if (angle == 0)
		return;
	this->x = cosf(angle * PI / 180) * this->x + sinf(angle * PI / 180)
			* this->z;
	this->z = -sin(angle * PI / 180) * this->x + cosf(angle * PI / 180)
			* this->z;
}

void Vector3::rotateZ(GLfloat angle) {
	if (angle == 0)
		return;
	this->x = cosf(angle * PI / 180) * this->x - sinf(angle * PI / 180)
			* this->y;
	this->y = sin(angle * PI / 180) * this->x + cosf(angle * PI / 180)
			* this->y;
}

Vector3 Vector3::rotateArbitrary(GLfloat angle, Vector3 vec) {
	GLfloat c = cosf(angle * PI / 180);
	GLfloat s = sinf(angle * PI / 180);
	GLfloat X = vec[0];
	GLfloat Y = vec[1];
	GLfloat Z = vec[2];

	return Vector3((c + (1 - c) * X * X) * x + ((1 - c) * Y * X - s * Z) * y
			+ ((1 - c) * Z * X + s * Y) * z, ((1 - c) * X * Y + s * Z) * x + (c
			+ (1 - c) * Y * Y) * y + ((1 - c) * Z * Y - s * X) * z, ((1 - c)
			* X * Z - s * Y) * x + ((1 - c) * Y * Z + s * X) * y + (c + (1 - c)
			* Z * Z) * z);
}

////////////////////////////////////////////////////////////
Vector4::Vector4() {
	x = y = z = w = 0;
}

Vector4::Vector4(GLfloat xpos, GLfloat ypos, GLfloat zpos, GLfloat wpos) {
	x = xpos;
	y = ypos;
	z = zpos;
	w = wpos;
}

inline GLfloat Vector4::W() {
	return w;
}

void Vector4::setW(GLfloat val) {
	w = val;
}

//dot product
GLfloat Vector4::operator *(Vector4 rhs) {
	return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z + this->w
			* rhs.w;
}

GLfloat Vector4::operator *(Vector3 rhs) {
	return this->x * rhs[0] + this->y * rhs[1] + this->z * rhs[2];
}

Vector4 Vector4::operator =(Vector4 rhs) {
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	this->w = rhs.w;
	return *this;
}

Vector4 Vector4::operator =(Vector3 rhs) {
	this->x = rhs[0];
	this->y = rhs[1];
	this->z = rhs[2];
	this->w = 0.0;
	return *this;
}
