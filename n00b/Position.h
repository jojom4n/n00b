#pragma once
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <vector>
#include <list>
#include <array>
#include <assert.h>
#include "defs.h"

class Position
{
	std::array<std::array<Bitboard, 6>, 2> board_;
	Bitboard whitePieces_{}, blackPieces_{}, allPieces_{};
	Square enPassantSquare_ {SQ_EMPTY};
	Color turn_ = WHITE; // who has the move?
	ushort moveNumber_ = 1; // number of moves. Default to 1
	ushort halfMove_ = 0; // number of half-moves. Default to 0
	bool checkmate_ = false; // is the player checkmated?
	std::array<ushort, 2> castle_ {ALL, ALL}; // castling rights for each player. Default to all
	std::array<ushort, 2> playerTime_ {600}; // time in seconds. Default to 10 mins

public:
	Position();
	~Position();

	void setNew();
	void resetPosition();
	
	constexpr Color getTurn() const { return turn_; }
	void setTurn(Color const& b) { turn_ = b; }

	constexpr Square getEnPassant() const { return enPassantSquare_; }
	void setEnPassant(const Square &square) { enPassantSquare_ = square; }
	
	constexpr ushort getMoveNumber() const { return moveNumber_; }
	void setMoveNumber(ushort const &move) { moveNumber_ = move; }

	constexpr ushort getHalfMove() const { return halfMove_; }
	void setHalfMove(ushort const &half_move) { halfMove_ = half_move; }
	
	constexpr bool getCheckmate() const { return checkmate_; }
	void setCheckmate(bool const &b) { checkmate_ = b; }

	constexpr ushort getCastle(Color const &color) const { return castle_[color]; }
	void setCastle(Color const &color, ushort const &castle) 
		{ (color == WHITE) ? castle_[WHITE] = castle : castle_[BLACK] = castle; }

	constexpr ushort getPlayerTime(Color const &color) const 
		{ return (color == WHITE) ? playerTime_[WHITE] : playerTime_[BLACK]; }
	void setPlayerTime(Color const &color, ushort time)
		{ (color == WHITE) ? playerTime_[WHITE] = time : playerTime_[BLACK] = time;	}
	
	void putPiece(Color const &color, Piece const &piece, Square const &square);
	void removePiece(Color const &color, Piece const &piece, Square const &square);
	
	constexpr Bitboard getPosition() const { return allPieces_; }
	constexpr Bitboard getPosition(Color const &color) const 
		{ return (color == WHITE) ? whitePieces_ : blackPieces_; }
	
	constexpr Bitboard getPieces(Color const &color, Piece const &piece) const 
		{ return board_[color][piece]; }
	
	constexpr bool occupiedSquare(Square const &square) const 
		{ return (allPieces_ & (C64(1) << square)) ? true : false; }

	const PieceID idPiece(Square const &square) const;

	const ushort count(Color const &color = ALL_COLOR) const;
	const ushort countPieceType(Color const &color, Piece const &piece) const;

	const std::vector<Square> getPieceOnSquare(Color const &color, Piece const &piece) const;
};
#endif