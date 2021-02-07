#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "enums.h"
#include <vector>

class Position;

const std::vector<Move> moveGeneration(Position const& p);
const std::vector<Move> generateOnlyKing(Color const& c, Position const& p);
const Bitboard pawnMoves(Position const& p, Square const& from, Color const &c, Bitboard const& occ, Bitboard const& own);
void castleMoves(Position const& p, std::vector<Move>& moveList);
void enPassant(Position const& p, Square const& enPassant, Color const& c, std::vector<Move>& moveList);
const MoveType setType(Piece const& piece, Bitboard const& occ, Color const& c, Square const& from, Square const& to);
const Move composeMove(Square const& from, Square const& to, Color const& c, ushort const& p, MoveType const& type,
	Piece const& captured, ushort const& promoteTo);
ushort underCheck(Color const& c, Position const& p);
void pruneIllegal(std::vector<Move>& moveList, Position& p);
const std::vector<Move> moveGenQS(Position const& p);

#endif
