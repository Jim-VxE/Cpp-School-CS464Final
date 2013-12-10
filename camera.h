
#include "mat.h"

#ifndef _CAMERA_CLASS_
#define _CAMERA_CLASS_

class Camera {
public:
	Camera::Camera(vec3 veye, vec3 vat);

	Angel::mat4 look(void);
	Angel::mat4 frustum(void);
	vec3 gauche(void);
	void aspect(int,int);
	void pos(vec3 p);
	void aim(vec3 t);

	void slideLeft(GLfloat d);
	void slideRight(GLfloat d);
	void slideFwd(GLfloat d);
	void slideBak(GLfloat d);
	void slideUp(GLfloat d);
	void slideDown(GLfloat d);
	void panLeft(GLfloat d);
	void panRight(GLfloat d);
	void panUp(GLfloat d);
	void panDown(GLfloat d);

	void bellyDown(void);
//private:
	vec3 eye, at, up;
	GLfloat left, right, bottom, top, zNear, zFar;
};
#endif