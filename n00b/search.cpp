#include "pch.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include "protos.h"
#include "Position.h"

const Move iterativeSearch (Position &p, ushort depth)
{
	unsigned long nodes{};
	bool flagMate{ true };
	Move m{};
	std::vector<Move> moveList = moveGeneration(p);
	moveList = pruneIllegal(moveList, p);

	for (short i = 1; i <= depth && flagMate; i++) {
		short bestScore = -MATE;
		std::vector<Move> pv{};
		pv.clear();
		
		auto t1 = Clock::now();
		m = negamaxRoot(p, i, bestScore, nodes, pv, moveList);
		auto t2 = Clock::now();

		if (bestScore == MATE || bestScore == -MATE)
			flagMate = false;
		
		std::chrono::duration<float, std::milli> time = t2 - t1;

		if (m) {
			std::cout << "\n*depth:" << i << " nodes:" << nodes << " ms:" << int(time.count()) << " nps:" << int(nodes / (time.count() / 1000)) << std::endl;
			std::cout << "\t move:" << displayMove(p, m) << " score:" << bestScore << " pv:";

			for (auto it = pv.begin(); it != pv.end(); ++it) {
				(it == std::prev(pv.end()) && (bestScore == MATE || bestScore == -MATE)) ? std::cout << displayMove(p, *it) << "# "
					: std::cout << displayMove(p, *it) << " ";
			}
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

const Move negamaxRoot(Position const& p, ushort depth, short &bestScore, unsigned long &nodes, std::vector<Move> &pv, std::vector<Move> &moveList) 
{
	Position copy = p;
	Move bestMove{};
		
	for (const auto& m : moveList) {
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

		if (score == MATE)
			return bestMove;
	}
	
	//put the best move found to the beginning of movelist for further depth search
	std::vector<Move>::iterator it = std::find(moveList.begin(), moveList.end(), bestMove);
	std::rotate(moveList.begin(), it, it + 1);

	return bestMove;
}


const short negamaxAB(Position const& p, ushort depth, unsigned long &nodes, short alpha, short beta, std::vector<Move> &childPv)
{
	Position copy = p;

	if (depth == 0)
		return quiescence(copy, alpha, beta, nodes);
		// return evaluate(p);

	short bestScore = -MATE;	
	Move bestMove{};
	std::vector<Move> MoveList = moveGeneration(copy);
	MoveList = pruneIllegal(MoveList, copy);
	
	for (const auto& m : MoveList) {
		short score{};
		std::vector<Move> nephewPv{};
		doMove(m, copy);
		score = -negamaxAB(copy, depth - 1, nodes, -beta, -alpha, nephewPv);
		undoMove(m, copy, p);
		nodes++;
				
		if (score > bestScore) {
			bestScore = score;
			bestMove = m;
		}	
		
		if (score > alpha) {
			alpha = score;
			childPv.clear();
			childPv.push_back(m);
			std::copy(nephewPv.begin(), nephewPv.end(), back_inserter(childPv));
		}

		if (alpha >= beta)
			return alpha;
	}
	
	return bestScore;
}


const short quiescence(Position const& p, short alpha, short beta, unsigned long &nodes)
{
	short stand_pat = evaluate(p);

	if (stand_pat >= beta)
		return beta;

	if (alpha < stand_pat)
		alpha = stand_pat;

	Position copy = p;
	std::vector<Move> moveList = moveGenQS(copy);
	moveList = pruneIllegal(moveList, copy);

	for (const auto& m : moveList) {
		
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