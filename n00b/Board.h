#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "defs.h"

class Board
{
	Bitboard bb[2][6];
	Bitboard white_pieces = 0ULL, black_pieces = 0ULL, all_pieces = 0ULL;
	
public:
	Board();
	~Board();
	
	void init(std::string const &fen) const; // TODO - Fetch position from FEN
	
	Bitboard get_piece_table(Color const &a, Piece const &b) const
		{return bb[a][b];}

	void set_piece(Color const &a, Piece const &b, Square const &c);
	void set_newgame();

	Bitboard get_position() const { return all_pieces; }
	Bitboard get_position(Color const &a) const;
	void update_bitboards(Color const &a);
	Bitboard get_square(Square const& a) { return all_pieces & (1ULL << a); }
};

#endif