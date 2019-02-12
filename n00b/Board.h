#pragma once
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <vector>
#include <array>
#include <assert.h>
#include "defs.h"

class Board
{
	std::array<std::array<Bitboard, 6>, 2> board_;
	Bitboard whitePieces_ = C64(0), blackPieces_ = C64(0), allPieces_ = C64(0);
	bool move_ = WHITE; // who has the move?
	bool checkmate_ = false; // is the player checkmated?

	struct Castle {
		Color player;
		enum side : const ushort { KINGSIDE, QUEENSIDE, BOTH, NONE };
	} castling_;
	
	void update(Color const &color);

public:
	Board();
	~Board();

	void setNew();

	constexpr bool getMove() const { return move_; }
	void setMove(bool const& b) { move_ = b; }

	constexpr bool getCheckmate() const { return checkmate_; }
	void setCheckmate(bool const &b) { checkmate_ = b; }

	constexpr Castle getCastle() const { return castling_; }
	void setCastle(Castle const &castle) { castling_ = castle; }

	void putPiece(Color const &color, Piece const &piece, Square const &square);

	constexpr Bitboard getPosition() const { return allPieces_; }
	constexpr Bitboard getPosition(Color const &color) const { return (color == WHITE) ? whitePieces_ : blackPieces_; }
	
	constexpr Bitboard getPieces(Color const &color, Piece const &piece) const { return board_[color][piece]; }

	constexpr Bitboard getOpponentBlockers() const
		{ return (move_ == WHITE) ? blackPieces_ : whitePieces_; }
	constexpr Bitboard getPlayerBlockers() const
		{ return (move_ == WHITE) ? whitePieces_ : blackPieces_; }
	constexpr Bitboard getAllBlockers() const
		{ return allPieces_;	}
	
	constexpr bool occupiedSquare(Square const &square) const { return (allPieces_ & (C64(1) << square)) ? true : false; }

	const coords idPiece(Square const &square) const;

	const ushort count(Color const &color = NO_COLOR) const;
	const ushort countPieceType(Color const &color, Piece const &piece) const;

	const std::vector<Square> getPieceOnSquare(Color const &color, Piece const &piece) const;
};

#endif