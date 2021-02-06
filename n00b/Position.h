#ifndef POSITION_H
#define POSITION_H

#include "defs.h"
#include "enums.h"
#include "params.h"
#include "zobrist.h"
#include <array>
#include <vector>


class Position
{
	std::array<std::array<Bitboard, 6>, 2> board_{};
	Bitboard whitePieces_{}, blackPieces_{}, allPieces_{};
	Square enPassantSquare_{};
	Color turn_{}; // who has the move?
	ushort moveNumber_{}; // number of moves. Default to 1 (see Position.cpp)
	ushort halfMove_{}; // number of half-moves. Default to 0 (see Position.cpp)
	bool checkmate_{ false }; // is the player checkmated?
	std::array<Castle, 2> castle_{}; // castling rights for each player. Default to all
	uint64_t zobristHash_{};

	struct prvState { // for undoing moves
		Color prvTurn_{};
		ushort prvMoveNumber_{}, prvHalfMove{};
		Square prvEnPassantSquare_{};
		std::array<Castle, 2> prvCastle_{};
		uint64_t prvZobristHash_{};
	} prvState[MAX_PLY];


public:
	Position();
	~Position();

	void setNew();
	void resetPosition();
	void storeState(ushort const &depth = 0);
	void restoreState(ushort const &depth = 0);

	constexpr Color getTurn() const { return turn_; }
	void setTurn(Color const &b) { turn_ = b; }

	constexpr Square getEnPassant() const { return enPassantSquare_; }
	void setEnPassant(const Square &square) { enPassantSquare_ = square; }

	constexpr ushort getMoveNumber() const { return moveNumber_; }
	void setMoveNumber(ushort const &move) { moveNumber_ = move; }

	constexpr ushort getHalfMove() const { return halfMove_; }
	void setHalfMove(ushort const &half_move) { halfMove_ = half_move; }

	constexpr bool getCheckmate() const { return checkmate_; }
	void setCheckmate(bool const &b) { checkmate_ = b; }

	constexpr Castle getCastle(Color const &color) const { return castle_[color]; }
	
	void setCastle(Color const &color, Castle const &castle)
	{
		(color == WHITE) ? castle_[WHITE] = castle : castle_[BLACK] = castle;
	}

	void putPiece(Color const &color, Piece const &piece, Square const &square);
	void removePiece(Color const &color, Piece const &piece, Square const &square);
		
	constexpr Bitboard getPosition() const { return allPieces_; }
	
	constexpr Bitboard getPosition(Color const &color) const
	{
		return (color == WHITE) ? whitePieces_ : blackPieces_;
	}

	constexpr Bitboard getPieces(Color const &color, Piece const &piece) const
	{
		return board_[color][piece];
	}

	constexpr bool occupiedSquare(Square const &square) const
	{
		return (allPieces_ & (C64(1) << square)) ? true : false;
	}

	const PieceID idPiece(Square const &square, Color const &color = ALL_COLOR) const;
	
	const ushort count(Color const &color = ALL_COLOR) const;
	const ushort countPieceType(Color const &color, Piece const &piece) const;
	
	const bool isEnding() const;

	const std::vector<Square> getPieceOnSquare(Color const &color, Piece const &piece) const;

	const bool isSquareAttacked(Color const &color, Square const &square) const;

	constexpr uint64_t getZobrist() const { return zobristHash_; }
	void setZobrist() { zobristHash_ = Zobrist::fill(*this); }
	
	void updateZobrist(Color const& c, Piece const& p, Square const& sq)
		{ zobristHash_ ^= Zobrist::getKey(c, p, sq); }
	void updateZobrist(Square const& sq)
		{ zobristHash_ ^= Zobrist::getKey(sq); }
	void updateZobrist(Color const& c)
		{ zobristHash_ ^= Zobrist::getKey(c); }
	void updateZobrist(Color const& c, Castle const& castle)
		{ zobristHash_ ^= Zobrist::getKey(c, castle); }
};

#endif