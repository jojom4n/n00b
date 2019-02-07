#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "defs.h"

class Board
{
	Bitboard bb[2][6];
	Bitboard white_pieces = 0ULL, black_pieces = 0ULL, all_pieces = 0ULL;
	bool move = white; // who has the move
	bool checkmated = false; // is the player checkmated?
	Castle castling = castle_both; // can the player castle?
	
public:
	Board();
	~Board();
	
	void set_newgame();

	bool has_move() const { return move; }
	void has_move(bool const& b) { move = b; }

	bool is_checkmated() const { return checkmated; }
	void is_checkmated(bool const &b) { checkmated = b; }

	Castle has_castling() const { return castling; }
	void has_castling(Castle const &castle) { castling = castle; }

	void set_piece(Color const &color, Piece const &piece_table, Square const &square);
	
	Bitboard get_position() const { return all_pieces; }
	Bitboard get_position(Color const &color) const;
	Bitboard get_position(Color const &color, Piece const &piece_table) const
		{ return bb[color][piece_table]; }

	Bitboard get_square(Square const& square) const { return all_pieces & (1ULL << square); }
	
	void update_bitboards(Color const &color);
};

#endif