#pragma once
#ifndef EVALUATION_H
#define EVALUATION_H

#include "defs.h"

class Evaluation
{
	const struct PieceValue_ {
		const ushort pawn{ 1 }, knight{ 3 }, bishop{ 3 }, rook{ 5 }, queen{ 9 }, king{ 200 };
	};

	std::array<int, 2> Material;
	int score;


public:
	Evaluation();
	~Evaluation();
};

#endif
