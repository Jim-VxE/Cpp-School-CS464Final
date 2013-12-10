
#include "camera.h"

Camera::Camera(vec3 veye, vec3 vat) {
	eye = veye;
	at = normalize(vat - eye);
	if (abs(at.y) < 0.997)
	{
		up = at;
		up.y *= -1;
		up = cross(at, cross(up, at));
	}
	else
		up = normalize(cross(at, vec3(at.y, 0.0, 0.0)));

	left = -1.0;
	right = 1.0;
	bottom = -1.0;
	top = 1.0;
	zNear = 1.428;
	zFar = 101.428;
}

void Camera::aspect(int w, int h) {
	GLfloat a;
	if (h > 0)
	{
		a = GLfloat(w) / h;
		right = a;
		left = -a;
		top = 1.0;
		bottom = -1.0;
	}
}

void Camera::pos(vec3 p) {
	eye = p;
}

void Camera::aim(vec3 p) {
	at = normalize(p - eye);
}

Angel::mat4 Camera::look(void) {
	return LookAt(eye, eye + at, up);
}

Angel::mat4 Camera::frustum(void) {
	return Frustum(left, right, bottom, top, zNear, zFar);;
}

vec3 Camera::gauche(void) {
	return normalize(cross(up, at));
}

void Camera::slideLeft(GLfloat d) {
	vec3 v = this->gauche() * d * 5;
	eye += v;
}

void Camera::slideRight(GLfloat d) {
	this->slideLeft(-d);
}

void Camera::slideFwd(GLfloat d) {
	vec3 v = at;
	if (abs(at.y) < 0.993)
		v.y = 0.0;
	eye += normalize(v) * d * 5;
}

void Camera::slideBak(GLfloat d) {
	this->slideFwd(-d);
}

void Camera::slideUp(GLfloat d) {
	eye += vec3(0.0,1.0,0.0) * d * 5;
}

void Camera::slideDown(GLfloat d) {
	this->slideUp(-d);
}

void Camera::panLeft(GLfloat d) {
	at = normalize(at + (this->gauche() * d / M_PI));
	this->bellyDown();
}

void Camera::panRight(GLfloat d) {
	this->panLeft(-d);
}

void Camera::panUp(GLfloat d) {
	at = normalize(at + (up * d / M_PI));
	up = cross(at, this->gauche());
	this->bellyDown();
}

void Camera::panDown(GLfloat d) {
	this->panUp(-d);
}

void Camera::bellyDown(void) {
	if (abs(at.y) < 0.993)
	{
		vec3 v = this->at;
		v.y += 2.0;
		up = normalize( cross(at, cross(v, at)) );
	}
}