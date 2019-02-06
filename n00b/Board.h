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
	
	Bitboard get_piece_table(Color const &a, Piece const &b) const
		{return bb[a][b];}

	void set_piece(Color const &a, Piece const &b, Square const &c);
	void set_piece(Color const &a, Piece const&b, Square const &from, Square const &to);

	void set_newgame();

	bool has_move() const { return move; }
	void set_move(bool const& b) { move = b; }

	Castle has_castling() const { return castling; }
	void has_castling(Castle const &i) { castling = i; }

	Bitboard get_position() const { return all_pieces; }
	Bitboard get_position(Color const &a) const;

	bool is_checkmated() const { return checkmated; }
	void is_checkmated(bool const &b) { checkmated = b; }

	void update_bitboards(Color const &a);

	Bitboard get_square(Square const& a) const { return all_pieces & (1ULL << a); }
};

#endif