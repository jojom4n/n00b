#include "pch.h"
#include "protos.h"
#include "psqt.h"
#include "Position.h"

const std::map <Piece, ushort> pieceValue_
{ {PAWN, 100}, {KNIGHT, 300}, {BISHOP, 320}, {ROOK, 500}, {QUEEN, 900}, {KING, 2000} };


const short evaluate(Position const &p)
{
	short whoToMove{};
	(p.getTurn() == WHITE) ? whoToMove = 1 : whoToMove = -1;

	return (evMaterial(p) + evPSQT(p)) * whoToMove;
}


const ushort evMaterial (Position const &p)
{
	std::array<ushort, 2> materialScore{};

	for (Color c = BLACK; c <= WHITE; c++) {
		ushort numRook{}, numBishop{}, numKnight{}, numQueen{}, numPawn{};
		numRook = p.countPieceType(c, ROOK);
		numBishop = p.countPieceType(c, BISHOP);
		numKnight = p.countPieceType(c, KNIGHT);
		numQueen = p.countPieceType(c, QUEEN);
		numPawn = p.countPieceType(c, PAWN);

		materialScore[c] = pieceValue_.at(ROOK) * numRook
			+ pieceValue_.at(BISHOP) * numBishop
			+ pieceValue_.at(KNIGHT) * numKnight
			+ pieceValue_.at(QUEEN) * numQueen
			+ pieceValue_.at(PAWN) * numPawn;
	}

	return materialScore[p.getTurn()] - materialScore[!p.getTurn()];	
}


const short evPSQT(Position const &p)
{
	std::array<short, 2> scorePSQT{};

	for (Color c = BLACK; c <= WHITE; c++) {
		for (Piece piece = KING; piece <= PAWN; piece++)
		{
			for (auto& sq : p.getPieceOnSquare(c, piece)) {
				scorePSQT[WHITE] += PSQT[piece][ushort(sq)];
				ushort index = (sq + 56) - ((sq / 8) * 16);
				scorePSQT[BLACK] += PSQT[piece][index];
			}
		}
	}
	return scorePSQT[p.getTurn()] - scorePSQT[!p.getTurn()];
}
