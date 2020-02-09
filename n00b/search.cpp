#include "pch.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include "protos.h"
#include "Position.h"

const Move searchRoot(Position const& p, short depth) 
{
	short bestScore = -(std::numeric_limits<short>::max());
	Position copy = p;
	Move bestMove{};
	
	std::vector<Move> moveList = moveGeneration(copy);
	moveList = pruneIllegal(moveList, copy);

	for (auto& m : moveList) {
		short score{};
		doMove(m, copy);
		score = -negamax(copy, depth - 1);

		if (score >= bestScore) {
			bestScore = score;
			bestMove = m;
		}

		undoMove(m, copy, p);
	}

	return bestMove;
}


const short negamax(Position const& p, short depth)
{
	if (depth == 0)
		return evaluate(p);

	short bestScore = -(std::numeric_limits<short>::max());
	Position copy = p;
	
	std::vector<Move> moveList = moveGeneration(copy);
	moveList = pruneIllegal(moveList, copy);

	for (auto& m : moveList) {
		short score{};
		doMove(m, copy);
		score = -negamax(copy, depth - 1);

		if (score > bestScore) 
			bestScore = score;
		
		undoMove(m, copy, p);
	}
	
	return bestScore;
}




/* 
const Move calculateBestMove(Position const& p, short depth, bool maxim) 
{

	Position copy = p;
	Color turn = copy.getTurn();
	std::vector<Move> moveList = moveGeneration(copy);
	Move bestMove{};
	short bestValue = -9999;
	short iterativeDepth{};

	for (ushort iterativeDepth = 1; iterativeDepth <= depth; iterativeDepth++) {
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
		std::vector<Move>::iterator it = std::find(moveList.begin(), moveList.end(), bestMove);
		std::rotate(moveList.begin(), it, it + 1);
	}
	return bestMove;
}


const int alphaBeta(Position const& p, short depth, int  alpha, int beta, bool maxim)
{
	Position copy = p;
	Color turn = copy.getTurn();
	std::vector<Move> moveList = moveGeneration(copy);

	if (depth == 0) {
		return evaluate(p);
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
} */