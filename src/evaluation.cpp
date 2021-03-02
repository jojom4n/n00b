#include "pch.h"
#include "evaluation.h"
#include "overloading.h"
#include "psqt.h"
#include "Position.h"

const std::map <Piece, ushort> g_pieceValue
{ {PAWN, 100}, { KNIGHT, 300 }, { BISHOP, 320 }, { ROOK, 500 }, { QUEEN, 900 }, { KING, 5000 } };


const short evaluate(Position const &p)
{
	short whoToMove{};
	(p.getTurn() == WHITE) ? whoToMove = 1 : whoToMove = -1; 

	return (evMaterial(p) + evPSQT(p)) * whoToMove;
}


const short evMaterial (Position const &p)
{
	std::array<ushort, 2> materialScore{};

	for (Color c = BLACK; c <= WHITE; c++) {
		ushort numRook{}, numBishop{}, numKnight{}, numQueen{}, numPawn{}, numKing{};
		numRook = p.countPieceType(c, ROOK);
		numBishop = p.countPieceType(c, BISHOP);
		numKnight = p.countPieceType(c, KNIGHT);
		numQueen = p.countPieceType(c, QUEEN);
		numPawn = p.countPieceType(c, PAWN);
		numKing = p.countPieceType(c, KING);

		materialScore[c] = g_pieceValue.at(ROOK) * numRook
			+ g_pieceValue.at(BISHOP) * numBishop
			+ g_pieceValue.at(KNIGHT) * numKnight
			+ g_pieceValue.at(QUEEN) * numQueen
			+ g_pieceValue.at(PAWN) * numPawn
			+ g_pieceValue.at(KING) * numKing;
	}

	return materialScore[WHITE] - materialScore[BLACK];	
}


const short evPSQT(Position const &p)
{
	std::array<short, ALL_COLOR> scorePSQT{};

	for (Color c = BLACK; c <= WHITE; c++) {
		for (Piece piece = KING; piece <= PAWN; piece++)
		{
			for (const auto& sq : p.getSquareOfPiece(c, piece)) {
				
				if (c == WHITE) 
					scorePSQT[WHITE] += PSQT[piece][sq];
				else 
					scorePSQT[BLACK] += PSQT[piece][sq ^ 0x38]; // flip PSQT for black

			}
		}
	}
	return scorePSQT[WHITE] - scorePSQT[BLACK];
}
