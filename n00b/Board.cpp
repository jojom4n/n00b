#include "pch.h"
#include <iostream>
#include "Board.h"

Board::Board()
{
}

Board::~Board()
{
	// we initialize main bitboard
	for (unsigned short i = 0; i < 1; i++)
		for (unsigned short z = 0; z < 5; z++)
			bb[i][z] = 0;
}

void Board::init(const std::string fen)
{
	// TODO - Fetch position from FEN
}
