#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

class Board
{
	// let's define the bitboards...
	typedef uint64_t Bitboard;
	
	// ...and let's create those bitboards;
	Bitboard bb[2][6];
	Bitboard White_Pieces = 0, Black_Pieces = 0, All_Pieces = 0;
	
public:
	Board();
	~Board();
	void init(const std::string fen); // TODO - Fetch position from FEN
};

#endif