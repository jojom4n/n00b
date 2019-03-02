#include "pch.h"
#include <iostream>
#include "protos.h"
#include "Position.h"


int positions;

const short negamax(Position &p, short depth, short alpha, short beta)
{
	positions++;

	if (depth == 0)
		return evaluate(p);

	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;

	for (auto &m : moveList) {
		doMove(m, copy);
		int score = -negamax(p, depth - 1, -beta, -alpha);
		undoMove(m, copy, p);

		if (score > alpha)
			alpha = score;
	}
	
	return alpha;
}


const Move calculateBestMove(Position &p, short depth)
{
	std::vector<Move> moveList = moveGeneration(p);
	Move bestMove = 0;
	int maxScore = -(std::numeric_limits<int>::max());

	Position copy = p;

	for (auto &m : moveList) {
		doMove(m, copy);
		int score = -negamax(p, depth - 1, -(std::numeric_limits<short>::max()), +(std::numeric_limits<short>::max()));
		undoMove(m, copy, p);

		if (score > maxScore) {
			maxScore = score;
			bestMove = m;
		}
	}

	std::cout << positions << std::endl;
	return bestMove;
}