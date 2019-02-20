#pragma once
#ifndef EVALUATION_H
#define EVALUATION_H

#include "defs.h"

class Evaluation
{
	std::array<int, 2> material_;
	const std::map <Piece, ushort> pieceValue_
		{ {PAWN, 1}, {KNIGHT, 3}, {BISHOP, 3}, {ROOK, 5}, {QUEEN, 9}, {KING, 200} };
	int score;


public:
	Evaluation();
	~Evaluation();
};

#endif
