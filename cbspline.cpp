
#include "cbspline.h"

vec4 cbspline(vec4 a, vec4 b, vec4 c, vec4 d, GLfloat t){
	GLfloat u = 1 - t;
	return vec4(a.x * u * u * u + 3 * b.x * u * u * t + 3 * c.x * u * t * t + d.x * t * t * t,
		a.y * u * u * u + 3 * b.y * u * u * t + 3 * c.y * u * t * t + d.y * t * t * t,
		a.z * u * u * u + 3 * b.z * u * u * t + 3 * c.z * u * t * t + d.z * t * t * t,
		a.w * u * u * u + 3 * b.w * u * u * t + 3 * c.w * u * t * t + d.w * t * t * t);
}