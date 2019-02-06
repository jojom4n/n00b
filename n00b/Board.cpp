#include "pch.h"
#include <iostream>
#include "Board.h"

Board::Board()
{
}

Board::~Board()
{
	// we initialize main bitboard
	for (unsigned short i = 0; i < 2; i++)
		for (unsigned short z = 0; z < 6; z++)
			bb[i][z] = 0;
}

void Board::init(std::string const& fen) const
{
	// TODO - Fetch position from FEN
}
