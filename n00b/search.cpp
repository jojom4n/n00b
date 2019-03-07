#include "pch.h"
#include <iostream>
#include <algorithm>
#include "protos.h"
#include "Position.h"

const Move calculateBestMove(Position const& p, short depth, bool maxim) 
{

	Position copy = p;
	Color turn = copy.getTurn();
	std::vector<Move> moveList = moveGeneration(copy);
	Move bestMove{};
	short bestValue = -9999;

	for (auto& m : moveList) {
		int value{};
		doMove(m, copy);
		if (underCheck(turn, copy) == 0) {
			value = alphaBeta(copy, depth - 1, -10000, 10000, !maxim);
		}
		else
		{
			undoMove(m, copy, p);
			continue;
		}
		undoMove(m, copy, p);

		if (value >= bestValue) {
			bestValue = value;
			bestMove = m;
		}
	}

	return bestMove;
}


const int alphaBeta(Position const& p, short depth, int  alpha, int beta, bool maxim)
{
	Position copy = p;
	Color turn = copy.getTurn();
	std::vector<Move> moveList = moveGeneration(copy);

	if (depth == 0) {
		return -evaluate(p);
	}

	if (maxim) {
		int bestValue = -9999;

		for (auto& m : moveList) {
			doMove(m, copy);
			
			if (underCheck(turn, copy) == 0) 
				bestValue = std::max(bestValue, alphaBeta(copy, depth - 1, alpha, beta, !maxim));
			else {
				undoMove(m, copy, p);
				continue;
			}
			
			undoMove(m, copy, p);
			alpha = std::max(alpha, bestValue);

			if (beta <= alpha)
				return bestValue;
		}

		//std::cout << bestValue << "\t\n";
		return bestValue;
	}
	else {
		int bestValue = 9999;

		for (auto& m : moveList) {
			doMove(m, copy);
			bestValue = std::min(bestValue, alphaBeta(copy, depth - 1, alpha, beta, !maxim));
			undoMove(m, copy, p);
			beta = std::min(beta, bestValue);

			if (beta <= alpha)
				return bestValue;
		}

		// std::cout << bestValue << "\t\n";
		return bestValue;
	}
}