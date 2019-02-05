#include "pch.h"
#include "board.h"

// default FEN position
const string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

Board::Board()
{
}


Board::~Board()
{
}

// we initizialize bitboards of chessboard with FEN provided by user;
// if no FEN is provided, we use the standard FEN of start position
void Board::init(const string fen = startFEN)
{
}
