#include "pch.h"
#include <iostream>
#include "Position.h"
#include "defs.h"
#include "protos.h"


Position::Position()
{

}


Position::~Position()
{
	
}


void Position::setNew()
{
	// white pieces
	putPiece(WHITE, KING, E1);
	putPiece(WHITE, QUEEN, D1);
	putPiece(WHITE, ROOK, A1);
	putPiece(WHITE, ROOK, H1);
	putPiece(WHITE, KNIGHT, B1);
	putPiece(WHITE, KNIGHT, G1);
	putPiece(WHITE, BISHOP, C1);
	putPiece(WHITE, BISHOP, F1);
	
	for (Square i = A2; i <= H2; i++)
		putPiece(WHITE, PAWN, i);

	//black pieces
	putPiece(BLACK, KING, E8);
	putPiece(BLACK, QUEEN, D8);
	putPiece(BLACK, ROOK, A8);
	putPiece(BLACK, ROOK, H8);
	putPiece(BLACK, KNIGHT, B8);
	putPiece(BLACK, KNIGHT, G8);
	putPiece(BLACK, BISHOP, C8);
	putPiece(BLACK, BISHOP, F8);

	for (Square i = A7; i <= H7; i++)
		putPiece(BLACK, PAWN, i);
}


void Position::resetPosition()
{
	board_.fill({});
	whitePieces_ = C64(0), blackPieces_ = C64(0), allPieces_ = C64(0);
}


void Position::putPiece(Color const &color, Piece const &piece, Square const &square)
{
	board_[color][piece] |= C64(1) << square;
	(color == WHITE) ? whitePieces_ |= C64(1) << square : blackPieces_ |= C64(1) << square;
	allPieces_ |= C64(1) << square;
}


void Position::removePiece(Color const &color, Piece const &piece, Square const &square)
{
	board_[color][piece] &= ~(C64(1) << square);
	(color == WHITE) ? whitePieces_ &= ~(C64(1) << square) : blackPieces_ &= ~(C64(1) << square);
	allPieces_ &= ~(C64(1) << square);
}


const PieceID Position::idPiece(Square const &square) const
{
	/* let us AND the bit set in the square (1ULL << square) with the main bitboards,
	until we find the one containing that bit */

	for (ushort x = 0; x < 2; x++)
		for (ushort y = 0; y < 6; y++) {
			if (board_[x][y] & (C64(1) << square))
				return PieceID{ (Color)x, (Piece)y };
		}

	return { ALL_COLOR, NO_PIECE };  // some error occurred
}


// see https://www.chessprogramming.org/Population_Count
const ushort Position::count(Color const &color) const 
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


const ushort Position::countPieceType(Color const &color, Piece const &piece) const
{
	Bitboard count = board_[color][piece];
	count = count - ((count >> 1) & k1);
	count = (count & k2) + ((count >> 2)  & k2);
	count = (count + (count >> 4)) & k4;
	count = (count * kf) >> 56;
	return (ushort)count;
}


const std::vector<Square> Position::getPieceOnSquare(Color const &color, Piece const &piece) const
{
	std::vector<Square> squares;
	Bitboard temp = board_[color][piece];

	while (temp > 0) 
		 squares.push_back(Square(bitscan_reset(temp)));
	
	return squares;
}