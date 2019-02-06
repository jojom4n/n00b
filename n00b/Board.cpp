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

void Board::set_piece(Color const &a, Piece const &b, Square const &c)
{
	bb[a][b] |= 1ULL << c;
	update_bitboards(a);
}

void Board::set_newgame()
{
	// white pieces
	set_piece(white, king, E1);
	set_piece(white, queen, D1);
	set_piece(white, rooks, A1);
	set_piece(white, rooks, H1);
	set_piece(white, knights, B1);
	set_piece(white, knights, G1);
	set_piece(white, bishops, C1);
	set_piece(white, bishops, F1);
	
	for (short int i = A2; i <= H2; i++)
		set_piece(white, pawns, Square(i));

	//black pieces
	set_piece(black, king, E8);
	set_piece(black, queen, D8);
	set_piece(black, rooks, A8);
	set_piece(black, rooks, H8);
	set_piece(black, knights, B8);
	set_piece(black, knights, G8);
	set_piece(black, bishops, C8);
	set_piece(black, bishops, F8);

	for (short int i = A7; i <= H7; i++)
		set_piece(black, pawns, Square(i));
}

Bitboard Board::get_position(Color const &a) const 
{
	if (a == white)
		return white_pieces;
	else if (a == black)
		return black_pieces;
}

void Board::update_bitboards(Color const &a)
{
	if (a == white)
		for (unsigned short i = 0; i < 6; i++)
			white_pieces = white_pieces | bb[white][i];
	else if (a == black)
		for (unsigned short i = 0; i < 6; i++)
			black_pieces = black_pieces | bb[black][i];

	all_pieces = white_pieces | black_pieces;
}

void Board::init(std::string const &fen) const
{
	// TODO - Fetch position from FEN
}