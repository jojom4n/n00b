#include "pch.h"
#include "defs.h"
#include "Position.h"
#include "protos.h"

const std::map <Piece, ushort> pieceValue_
{ {PAWN, 100}, {KNIGHT, 300}, {BISHOP, 300}, {ROOK, 500}, {QUEEN, 900}, {KING, 2000} };


constexpr int PSQT[6][64]
{

// KING
{ -30,-40,-40,-50,-50,-40,-40,-30, 
-30,-40,-40,-50,-50,-40,-40,-30, 
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-20,-30,-30,-40,-40,-30,-30,-20,
-10,-20,-20,-20,-20,-20,-20,-10,
 20, 20,  0,  0,  0,  0, 20, 20,
 20, 30, 10,  0,  0, 10, 30, 20 },


 // QUEEN
{ -20,-10,-10, -5, -5,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5,  5,  5,  5,  0,-10,
 -5,  0,  5,  5,  5,  5,  0, -5,
  0,  0,  5,  5,  5,  5,  0, -5,
-10,  5,  5,  5,  5,  5,  0,-10,
-10,  0,  5,  0,  0,  0,  0,-10,
-20,-10,-10, -5, -5,-10,-10,-20 },


// ROOK
{ 0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
  0,  0,  0,  5,  5,  0,  0,  0 },


// KNIGHT
{ -50,-40,-30,-30,-30,-30,-40,-50,
-40,-20,  0,  0,  0,  0,-20,-40,
-30,  0, 10, 15, 15, 10,  0,-30,
-30,  5, 15, 20, 20, 15,  5,-30,
-30,  0, 15, 20, 20, 15,  0,-30,
-30,  5, 10, 15, 15, 10,  5,-30,
-40,-20,  0,  5,  5,  0,-20,-40,
-50,-40,-30,-30,-30,-30,-40,-50 },


// BISHOP
{ -20,-10,-10,-10,-10,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5, 10, 10,  5,  0,-10,
-10,  5,  5, 10, 10,  5,  5,-10,
-10,  0, 10, 10, 10, 10,  0,-10,
-10, 10, 10, 10, 10, 10, 10,-10,
-10,  5,  0,  0,  0,  0,  5,-10,
-20,-10,-10,-10,-10,-10,-10,-20 },


// PAWN
{ 0,  0,  0,  0,  0,  0,  0,  0,
50, 50, 50, 50, 50, 50, 50, 50,
10, 10, 20, 30, 30, 20, 10, 10,
 5,  5, 10, 25, 25, 10,  5,  5,
 0,  0,  0, 20, 20,  0,  0,  0,
 5, -5,-10,  0,  0,-10, -5,  5,
 5, 10, 10,-20,-20, 10, 10,  5,
 0,  0,  0,  0,  0,  0,  0,  0 }
};


int evaluate(Position const &pos)
{
	int score{}, who2move;
	(pos.getTurn() == WHITE) ? who2move = 1 : who2move = -1;
	score = evMaterial(pos) + evPSQT(pos);
	return score * who2move;
}


int evMaterial (Position const &pos)
{
	ushort numRookW{}, numBishopW{}, numKnightW{}, numQueenW{}, numPawnW{};
	ushort numRookB{}, numBishopB{}, numKnightB{}, numQueenB{}, numPawnB{};

	numRookW = pos.countPieceType(pos.getTurn(), ROOK);
	numBishopW = pos.countPieceType(pos.getTurn(), BISHOP);
	numKnightW = pos.countPieceType(pos.getTurn(), KNIGHT);
	numQueenW = pos.countPieceType(pos.getTurn(), QUEEN);
	numPawnW = pos.countPieceType(pos.getTurn(), PAWN);
	
	numRookB = pos.countPieceType(Color(!pos.getTurn()), ROOK);
	numBishopB = pos.countPieceType(Color(!pos.getTurn()), BISHOP);
	numKnightB = pos.countPieceType(Color(!pos.getTurn()), KNIGHT);
	numQueenB = pos.countPieceType(Color(!pos.getTurn()), QUEEN);
	numPawnB = pos.countPieceType(Color(!pos.getTurn()), PAWN);
	
	int materialScore = pieceValue_.at(ROOK) * (numRookW - numRookB)
		+ pieceValue_.at(BISHOP) * (numBishopW - numBishopB)
		+ pieceValue_.at(KNIGHT) * (numKnightW - numKnightB)
		+ pieceValue_.at(QUEEN) * (numQueenW - numKnightB)
		+ pieceValue_.at(PAWN) * (numPawnW - numPawnB);
	
	return materialScore;	
}


int evPSQT(Position const &p)
{
	int scorePSQT{};

	for (int i = 0; i < 6; i++)
	{
		std::vector<Square> tmp_psqt;
		tmp_psqt = p.getPieceOnSquare(p.getTurn(), Piece(i));

		for (auto &square : tmp_psqt)
			scorePSQT += PSQT[i][ushort(square)];
	}

	return scorePSQT;
}
