#include "pch.h"
#include <iostream>
#include "board.h"

Board::Board()
{
}

Board::~Board()
{
	
}

// we initizialize bitboards
void Board::init()
{
	for (unsigned short i = 0; i < 1; i++)
		for (unsigned short z = 0; z < 5; z++)
			bb[i][z] = 0;
}
