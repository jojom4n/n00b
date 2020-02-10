#include "pch.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include "protos.h"
#include "Position.h"

const Move searchRoot(Position const& p, short depth) 
{
	long nodes{};
	short bestScore = -SHRT_INFINITY;
	Position copy = p;
	Move bestMove{};
	
	std::vector<Move> moveList = moveGeneration(copy);
	moveList = pruneIllegal(moveList, copy);

	for (auto& m : moveList) {
		short score{};
		doMove(m, copy);
		score = -negamaxAB(copy, depth - 1, nodes, -BETA, -ALPHA);

		if (score >= bestScore) {
			bestScore = score;
			bestMove = m;
		}

		undoMove(m, copy, p);
		nodes++;

	}

	std::cout << "Nodes traversed: " << nodes << std::endl;
	std::cout << "Best score: " << bestScore << std::endl;
	return bestMove;
}


const short negamaxAB(Position const& p, short depth, long &nodes, short alpha, short beta)
{
	if (depth == 0)
		return quiescence(p, alpha, beta);

	Position copy = p;
	short bestScore = -SHRT_INFINITY;	
	std::vector<Move> moveList = moveGeneration(copy);
	moveList = pruneIllegal(moveList, copy);

	for (auto& m : moveList) {
		short score{};
		doMove(m, copy);
		score = -negamaxAB(copy, depth - 1, nodes, -beta, -alpha);

		if (score > bestScore) 
			bestScore = score;
		
		if (score > alpha)
			alpha = score;

		undoMove(m, copy, p);
		nodes++;

		if (alpha >= beta)
			return alpha;
	}
	
	return bestScore;
}


const short quiescence(Position const& p, short alpha, short beta)
{
	short stand_pat = evaluate(p);

	if (stand_pat >= beta)
		return beta;

	if (alpha < stand_pat)
		alpha = stand_pat;

	Position copy = p;
	std::vector<Move> moveList = moveGeneration(copy);
	moveList = pruneIllegal(moveList, copy);

	for (auto& m : moveList) {
		
		short score{};
		ushort moveType = ((C64(1) << 3) - 1) & (m >> 6); //check if move is a capture
		
		if (moveType == CAPTURE) {
			doMove(m, copy);
			score = -quiescence(copy, -beta, -alpha);
			undoMove(m, copy, p);

			if (score >= beta)
				return beta;

			if (score > alpha)
				alpha = score;
		}
	}

	return alpha;
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