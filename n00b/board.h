#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

class Board
{
	// let's define the bitboards...
	typedef uint64_t Bitboard;
	
	// ...and let's create those bitboards;
	Bitboard bb[2][6];
	
public:
	Board();
	~Board();
	void init();
};

#endif