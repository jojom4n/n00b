#include <iostream>
#include "pch.h"
#include "board.h"

using std::cout;
using std::endl;

Board::Board()
{
}

Board::~Board()
{
	W_King = 0;
	W_Queen = 0;
	W_Pawns = 0;
	W_Knights = 0;
	W_Bishops = 0;
	W_Rooks = 0;
	B_King = 0;
	B_Queen = 0;
	B_Pawns = 0;
	B_Knights = 0;
	B_Bishops = 0;
	B_Rooks = 0;
}

// we initizialize bitboards of chessboard with FEN provided by user;
// if no FEN is provided, we use the standard FEN of start position
void Board::init(const string fen)
{
	
}
