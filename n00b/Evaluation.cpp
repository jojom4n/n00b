#include "pch.h"
#include "defs.h"
#include "Position.h"
#include "protos.h"
#include "psqt.h"

const std::map <Piece, ushort> pieceValue_
{ {PAWN, 100}, {KNIGHT, 300}, {BISHOP, 300}, {ROOK, 500}, {QUEEN, 900}, {KING, 2000} };


int evaluate(Position const &pos)
{
	short scoreW{}, scoreB{};
	scoreW = evMaterial(pos, WHITE) + evPSQT(pos, WHITE);
	scoreB = evMaterial(pos, BLACK) + evPSQT(pos, BLACK);
	
	return (scoreW - scoreB);
}


short evMaterial (Position const &pos, Color const &color)
{
	ushort numRook{}, numBishop{}, numKnight{}, numQueen{}, numPawn{};

	numRook = pos.countPieceType(color, ROOK);
	numBishop = pos.countPieceType(color, BISHOP);
	numKnight = pos.countPieceType(color, KNIGHT);
	numQueen = pos.countPieceType(color, QUEEN);
	numPawn = pos.countPieceType(color, PAWN);
	
	short materialScore = pieceValue_.at(ROOK) * numRook
		+ pieceValue_.at(BISHOP) * numBishop
		+ pieceValue_.at(KNIGHT) * numKnight
		+ pieceValue_.at(QUEEN) * numQueen
		+ pieceValue_.at(PAWN) * numPawn;
	
	return materialScore;	
}


short evPSQT(Position const &p, Color const &color)
{
	short scorePSQT{};

 	for (int i = 0; i < 6; i++)
	{
		for (auto &square : p.getPieceOnSquare(color, Piece(i)))
			scorePSQT += PSQT[i][ushort(square)];
	}

	return scorePSQT;
}
