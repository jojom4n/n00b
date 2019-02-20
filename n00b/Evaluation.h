#pragma once
#ifndef EVALUATION_H
#define EVALUATION_H

#include "defs.h"
#include "Position.h"

class Evaluation
{
	std::array<int, 2> material_;
	const std::map <Piece, ushort> pieceValue_
		{ {PAWN, 1}, {KNIGHT, 3}, {BISHOP, 3}, {ROOK, 5}, {QUEEN, 9}, {KING, 200} };
	int score;

	int material(Position const &pos);
	int pieceSquareTable(Position const &pos);

public:
	Evaluation();
	~Evaluation();

	int getScore(Position const &pos);
	bool isCheck(Position const &pos);

};

#endif
