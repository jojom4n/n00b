#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <vector>
#include <assert.h>
#include "defs.h"

class Board
{
	Bitboard bb[2][6] = {};
	Bitboard white_pieces = C64(0), black_pieces = C64(0), all_pieces = C64(0);
	bool move = white; // who has the move
	bool checkmate = false; // is the player checkmated?

	struct Castle {
		Color player;
		enum side { castle_kingside, castle_queenside, castle_both, castle_none };
	} castling;

public:
	Board();
	~Board();

	void set_newgame();

	constexpr bool has_move() const { return move; }
	void has_move(bool const& b) { move = b; }

	constexpr bool is_checkmate() const { return checkmate; }
	void is_checkmate(bool const &b) { checkmate = b; }

	constexpr Castle has_castling() const { return castling; }
	void has_castling(Castle const &castle) { castling = castle; }

	void set_piece(Color const &color, Piece const &piece_table, Square const &square);

	constexpr Bitboard get_position() const { return all_pieces; }

	const Bitboard get_position(Color const &color) const
	{
		return (color == white) ? white_pieces : black_pieces;
	}

	constexpr Bitboard get_position(Color const &color, Piece const &piece_table) const
	{
		return bb[color][piece_table];
	}

	constexpr Bitboard is_square_occupied(Square const &square) const
	{
		return all_pieces & (C64(1) << square);
	}

	const bb_index identify_piece(Square const &square) const;

	const ushort popcount(Color const &color = no_color) const;
	const ushort popcount(Color const &color, Piece const &piece) const;

	const std::vector<Square> get_square(Color const &color, Piece const &piece);

	void update_bitboards(Color const &color);
};
#endif