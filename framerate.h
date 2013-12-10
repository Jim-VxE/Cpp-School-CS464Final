// A mechanism to limit the rate at which events should be processed
// Intended for use with openGL, e.g: "glutIdleFunc(framerateControl::idle)"
// Written by James McDonald

#include <ctime>

#ifndef _FRAMERATECONTROL_NS_
#define _FRAMERATECONTROL_NS_

namespace framerateControl
{
	void idle(void);
	void init(double fps, void(*fn)(double));
	extern clock_t ding;
	extern clock_t tick;
	extern void(*onFrame)(double);
}


#endif