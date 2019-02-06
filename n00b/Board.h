#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "defs.h"

class Board
{
	Bitboard bb[2][6];
	Bitboard White_Pieces = 0ULL, Black_Pieces = 0ULL, All_Pieces = 0ULL;
	
public:
	Board();
	~Board();
	
	void init(std::string const& fen) const; // TODO - Fetch position from FEN
	
	Bitboard get_piece(unsigned short const& x, unsigned short const& y) const
		{return bb[x][y];}

	Bitboard get_position() const { return All_Pieces; }
	Bitboard get_position(bool const& b) const;
};

#endif