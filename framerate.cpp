
#include "framerate.h"
#include <iostream>

namespace framerateControl
{
	clock_t ding;
	clock_t tick;
	void(*onFrame)(double);

	void init(double fps, void(*fn)(double)) {
		ding = (clock_t) (CLOCKS_PER_SEC / fps);
		tick = clock();
		onFrame = fn;
	}

	void idle(void) {
		clock_t tock = clock();
		//std::cout << (tick - tock) << '\n';
		if ( tick < tock )
		{
			while ( tick < tock )
				tick += ding;
			(*onFrame)((tick - tock) / ((double) CLOCKS_PER_SEC));
		}
	}
}