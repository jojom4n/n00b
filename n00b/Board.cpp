#include "pch.h"
#include <iostream>
#include "Board.h"
#include "defs.h"

Board::Board()
{
	// we initialize main bitboard
	for (unsigned short i = 0; i < 2; i++)
		for (unsigned short z = 0; z < 6; z++)
			bb[i][z] = 0ULL;
}

Board::~Board()
{
	
}

Bitboard Board::get_position(bool const& b) const 
{
	if (b == white)
		return White_Pieces;
	else if (b == black)
		return Black_Pieces;
}

void Board::init(std::string const& fen) const
{
	// TODO - Fetch position from FEN
}
