#include "pch.h"
#include <iostream>
#include "Position.h"
#include "defs.h"
#include "protos.h"

Position::Position()
{
	board_.fill({});
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
	
	for (ushort i = A2; i <= H2; i++)
		putPiece(WHITE, PAWN, Square(i));

	//black pieces
	putPiece(BLACK, KING, E8);
	putPiece(BLACK, QUEEN, D8);
	putPiece(BLACK, ROOK, A8);
	putPiece(BLACK, ROOK, H8);
	putPiece(BLACK, KNIGHT, B8);
	putPiece(BLACK, KNIGHT, G8);
	putPiece(BLACK, BISHOP, C8);
	putPiece(BLACK, BISHOP, F8);

	for (ushort i = A7; i <= H7; i++)
		putPiece(BLACK, PAWN, Square(i));
}


void Position::resetPosition()
{
	board_.fill({});
	whitePieces_= 0, blackPieces_ = 0, allPieces_ = 0;
}


void Position::putPiece(Color const &color, Piece const &piece, Square const &square)
{
	board_[color][piece] |= C64(1) << square;
	update(color);
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
	std::vector<Square> square;
	Bitboard count = board_[color][piece];
	
	while (count)
		{ square.push_back(Square(bitscan_reset(count))); }
	
	return square;
}


void Position::update(Color const &color)
{
	if (color == WHITE)
		for (ushort i = 0; i < 6; i++)
			whitePieces_ = whitePieces_ | board_[WHITE][i];
	else if (color == Color::BLACK)
		for (ushort i = 0; i < 6; i++)
			blackPieces_ = blackPieces_ | board_[BLACK][i];

	allPieces_ = whitePieces_ | blackPieces_;
}