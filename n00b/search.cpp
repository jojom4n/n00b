#include "pch.h"
#include <cmath>
#include <iostream>
#include "Position.h"
#include "protos.h"

int positions;

short negamax(Position &p, short depth, int alpha, int beta)
{
	positions++;

	if (depth == 0)
		return evaluate(p);

	std::vector<Move> moveList = moveGeneration(p);

	for (auto &m : moveList) {
		doMove(m, p);
		int score = -negamax(p, depth - 1, -beta, -alpha);
		undoMove(m, p);

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}

Move calculateBestMove(Position &p, short depth)
{
	std::vector<Move> moveList = moveGeneration(p);

	Move bestMove = 0;

	int maxScore = -10000;

	for (auto &m : moveList) {
		doMove(m, p);
		int score = -negamax(p, depth - 1, -INFINITY, +INFINITY);
		undoMove(m, p);

		if (score > maxScore)
		{
			maxScore = score;
			bestMove = m;
		}
	}

	std::cout << positions << std::endl;
	return bestMove;
}