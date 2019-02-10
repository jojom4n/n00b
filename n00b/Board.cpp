#include "pch.h"
#include <iostream>
#include "Board.h"
#include "defs.h"
#include "protos.h"

Board::Board()
{
	/* It SHOULD now work at initizialization time in Board.h. TEST!
	for (auto &row : bb)
		for (auto& elem : row)
			elem = C64(0);
	 */
}

Board::~Board()
{
	
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
	
	for (ushort i = A2; i <= H2; i++)
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

	for (ushort i = A7; i <= H7; i++)
		set_piece(black, pawns, Square(i));
}

void Board::set_piece(Color const &color, Piece const &piece_table, Square const &square)
{
	bb[color][piece_table] |= C64(1) << square;
	update_bitboards(color);
}

const bb_index Board::identify_piece(Square const &square) const
{
	Bitboard compare = C64(0); // create an empty bitboard for comparison...

	compare |= C64(1) << square; // ...and set (1) the single bit only from the square argument

	// let us ANDing the 'compare' bitboard with the main bitboards until we find the one
	// containing the same bit we set in 'compare'

	for (ushort x = 0; x < 2; x++)
		for (ushort y = 0; y < 6; y++)
			if (bb[x][y] & compare) {
				bb_index coords = { x, y };
				return coords;
			}

	return { NULL,NULL }; // some error occurred
}

// see https://www.chessprogramming.org/Population_Count
const ushort Board::popcount(Color const &color) const 
{
	Bitboard count;

	if (color == white)
		count = white_pieces;
	else if (color == black)
		count = black_pieces;
	else 
		count = all_pieces;
	
	count = count - ((count >> 1) & k1);
	count = (count & k2) + ((count >> 2)  & k2);
	count = (count + (count >> 4)) & k4;
	count = (count * kf) >> 56;
	return (ushort)count;
}

const ushort Board::popcount(Color const &color, Piece const &piece) const
{
	Bitboard count = bb[color][piece];
	count = count - ((count >> 1) & k1);
	count = (count & k2) + ((count >> 2)  & k2);
	count = (count + (count >> 4)) & k4;
	count = (count * kf) >> 56;
	return (ushort)count;
}


// TODO: First we must implement a bitscan_forward function at least
const std::vector<Square> Board::get_square(Color const &color, Piece const &piece)
{
	std::vector<Square> squares;
	Bitboard count = bb[color][piece];
	
	while (count)
		{ squares.push_back(bitscan_reset(count)); }
	
	return squares;
}

void Board::update_bitboards(Color const &color)
{
	if (color == white)
		for (ushort i = 0; i < 6; i++)
			white_pieces = white_pieces | bb[white][i];
	else if (color == black)
		for (ushort i = 0; i < 6; i++)
			black_pieces = black_pieces | bb[black][i];

	all_pieces = white_pieces | black_pieces;
}