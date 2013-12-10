

#include "keyStates.h"

namespace keyStates
{
	int key[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	bool get(unsigned char c){
		return (key[c >> 5] & (1 << (c & 31))) != 0;
	}

	void down(unsigned char c, int x, int y){
		if ('A' <= c && c <= 'Z')
			c |= 32;
		key[c >> 5] |= (1 << (c & 31));
	}

	void up(unsigned char c, int x, int y){
		if ('A' <= c && c <= 'Z')
			c |= 32;
		key[c >> 5] &= ~(1 << (c & 31));
	}
}