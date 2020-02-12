#include "pch.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include "protos.h"
#include "Position.h"

const Move iterativeSearch (Position &p, short depth)
{
	long nodes{};
	Move m{};
	std::vector<Move> moveList = moveGeneration(p);
	moveList = pruneIllegal(moveList, p);
	std::cout << std::endl;

	for (short i = 1; i <= depth; i++) {
		short bestScore = -SHRT_INFINITY;
		std::vector<Move> pv{};
		pv.clear();
		
		auto t1 = Clock::now();
		m = negamaxRoot(p, i, bestScore, nodes, pv, moveList);
		auto t2 = Clock::now();

		std::chrono::duration<float, std::milli> time = t2 - t1;

		if (m) {
			std::cout << "depth:" << i << "\tnodes:" << nodes << " ms:" << time.count() << " nps:" << nodes / (time.count() / 1000) << std::endl;
			std::cout << "move:" << displayMove(p, m) << " score:" << bestScore << " pv:";

			for (auto& m : pv) {
				std::cout << displayMove(p, m) << " ";
			}

			std::cout << std::endl << std::endl;
		}

		else if (!m && !underCheck(p.getTurn(), p)) {
			std::cout << "\nIt's STALEMATE!" << std::endl;
		}

		else if (!m && underCheck(p.getTurn(), p)) {
			p.setCheckmate(true);
			std::cout << "\nIt's CHECKMATE!" << std::endl;
		}
	}
	
	return m;
}

const Move negamaxRoot(Position const& p, short depth, short &bestScore, long &nodes, std::vector<Move> &pv, std::vector<Move> &moveList) 
{
	Position copy = p;
	Move bestMove{};
	
	for (auto& m : moveList) {
		std::vector<Move> childPv{};
		short score{};
		doMove(m, copy);
		score = -negamaxAB(copy, depth - 1, nodes, -BETA, -ALPHA, childPv);

		if (score >= bestScore) {
			bestScore = score;
			bestMove = m;
			pv.clear();
			pv.push_back(m);
			std::copy(childPv.begin(), childPv.end(), back_inserter(pv));
		}

		undoMove(m, copy, p);
		nodes++;
	}

	//put the best move found to the beginning of movelist for further depth search
	std::vector<Move>::iterator it = std::find(moveList.begin(), moveList.end(), bestMove);
	std::rotate(moveList.begin(), it, it + 1);

	return bestMove;
}


const short negamaxAB(Position const& p, short depth, long &nodes, short alpha, short beta, std::vector<Move> &childPv)
{
	Position copy = p;

	if (depth == 0)
		return quiescence(copy, alpha, beta, nodes);
		// return evaluate(p);

	short bestScore = -SHRT_INFINITY;	
	std::vector<Move> moveList = moveGeneration(copy);
	moveList = pruneIllegal(moveList, copy);

	for (auto& m : moveList) {
		short score{};
		std::vector<Move> nephewPv{};
		doMove(m, copy);
		score = -negamaxAB(copy, depth - 1, nodes, -beta, -alpha, nephewPv);

		if (score > bestScore) 
			bestScore = score;
			
		
		if (score > alpha) {
			alpha = score;
			childPv.clear();
			childPv.push_back(m);
			std::copy(nephewPv.begin(), nephewPv.end(), back_inserter(childPv));
		}

		undoMove(m, copy, p);
		nodes++;

		if (alpha >= beta)
			return alpha;
	}
	
	return bestScore;
}


const short quiescence(Position const& p, short alpha, short beta, long &nodes)
{
	short stand_pat = evaluate(p);

	if (stand_pat >= beta)
		return beta;

	if (alpha < stand_pat)
		alpha = stand_pat;

	Position copy = p;
	std::vector<Move> moveList = moveGenQS(copy);
	moveList = pruneIllegal(moveList, copy);

	for (auto& m : moveList) {
		
		short score{};
		doMove(m, copy);
		score = -quiescence(copy, -beta, -alpha, nodes);
		undoMove(m, copy, p);
		nodes++;

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
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