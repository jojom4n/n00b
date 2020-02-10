#include "pch.h"
#include "protos.h"
#include "psqt.h"
#include "Position.h"

const std::map <Piece, ushort> pieceValue_
{ {PAWN, 100}, {KNIGHT, 300}, {BISHOP, 320}, {ROOK, 500}, {QUEEN, 900}, {KING, SHRT_INFINITY} };


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

		materialScore[c] = pieceValue_.at(ROOK) * numRook
			+ pieceValue_.at(BISHOP) * numBishop
			+ pieceValue_.at(KNIGHT) * numKnight
			+ pieceValue_.at(QUEEN) * numQueen
			+ pieceValue_.at(PAWN) * numPawn
			+ pieceValue_.at(KING) * numKing;
	}

	return materialScore[WHITE] - materialScore[BLACK];	
}


const short evPSQT(Position const &p)
{
	std::array<short, 2> scorePSQT{};

	for (Color c = BLACK; c <= WHITE; c++) {
		for (Piece piece = KING; piece <= PAWN; piece++)
		{
			for (auto& sq : p.getPieceOnSquare(c, piece)) {
				
				if (c == WHITE) 
					scorePSQT[WHITE] += PSQT[piece][sq];
				else 
					scorePSQT[BLACK] += PSQT[piece][sq ^ 0x38]; // flip PSQT for black

			}
		}
	}
	return scorePSQT[WHITE] - scorePSQT[BLACK];
}
