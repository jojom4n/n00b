#include "pch.h"
#include <iostream>
#include "Board.h"
#include "defs.h"
#include "protos.h"

Board::Board()
{
	board_.fill({});
}

Board::~Board()
{
	
}

void Board::setNew()
{
	// white pieces
	putPiece(WHITE, KING, E1);
	putPiece(WHITE, QUEEN, D1);
	putPiece(WHITE, ROOKS, A1);
	putPiece(WHITE, ROOKS, H1);
	putPiece(WHITE, KNIGHTS, B1);
	putPiece(WHITE, KNIGHTS, G1);
	putPiece(WHITE, BISHOPS, C1);
	putPiece(WHITE, BISHOPS, F1);
	
	for (ushort i = A2; i <= H2; i++)
		putPiece(WHITE, PAWNS, Square(i));

	//black pieces
	putPiece(BLACK, KING, E8);
	putPiece(BLACK, QUEEN, D8);
	putPiece(BLACK, ROOKS, A8);
	putPiece(BLACK, ROOKS, H8);
	putPiece(BLACK, KNIGHTS, B8);
	putPiece(BLACK, KNIGHTS, G8);
	putPiece(BLACK, BISHOPS, C8);
	putPiece(BLACK, BISHOPS, F8);

	for (ushort i = A7; i <= H7; i++)
		putPiece(BLACK, PAWNS, Square(i));
}

void Board::putPiece(Color const &color, Piece const &piece, Square const &square)
{
	board_[color][piece] |= C64(1) << square;
	update(color);
}

const coords Board::idPiece(Square const &square) const
{
	/* let us AND the bit set in the square (1ULL << square) with the main bitboards,
	until we find the one containing that bit */

	for (ushort x = 0; x < 2; x++)
		for (ushort y = 0; y < 6; y++)
			if (board_[x][y] & (C64(1) << square))
				return coords{ x,y };
	
	return { NULL, NULL };  // some error occurred
}

// see https://www.chessprogramming.org/Population_Count
const ushort Board::count(Color const &color) const 
{
	uint64_t count;

	if (color == WHITE)
		count = whitePieces_;
	else if (color == BLACK)
		count = blackPieces_;
	else 
		count = allPieces_;
	
	count = count - ((count >> 1) & k1);
	count = (count & k2) + ((count >> 2)  & k2);
	count = (count + (count >> 4)) & k4;
	count = (count * kf) >> 56;
	return (ushort)count;
}

const ushort Board::countPieceType(Color const &color, Piece const &piece) const
{
	Bitboard count = board_[color][piece];
	count = count - ((count >> 1) & k1);
	count = (count & k2) + ((count >> 2)  & k2);
	count = (count + (count >> 4)) & k4;
	count = (count * kf) >> 56;
	return (ushort)count;
}


const std::vector<Square> Board::getPieceOnSquare(Color const &color, Piece const &piece) const
{
	std::vector<Square> square;
	Bitboard count = board_[color][piece];
	
	while (count)
		{ square.push_back(Square(bitscan_reset(count))); }
	
	return square;
}

void Board::update(Color const &color)
{
	if (color == WHITE)
		for (ushort i = 0; i < 6; i++)
			whitePieces_ = whitePieces_ | board_[WHITE][i];
	else if (color == BLACK)
		for (ushort i = 0; i < 6; i++)
			blackPieces_ = blackPieces_ | board_[BLACK][i];

	allPieces_ = whitePieces_ | blackPieces_;
}