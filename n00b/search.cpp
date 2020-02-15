#include "pch.h"
#include <iostream>
#include <iomanip>
#include "protos.h"
#include "Position.h"
#include "params.h"

struct Search {
	Position pos{};
	unsigned short ply{};
	short bestScore{};
	unsigned long nodes{};
	std::vector<Move> pv{};
	Move currBestMove{};
	Move bestMove;
	bool flagMate{};
};

const Move iterativeSearch (Position &p, ushort const depth)
{
	Search Search{};
	Search.bestScore = 0;
	Search.pos = p;
		
	for (short ply = 1; ply <= depth && !Search.flagMate; ply++) {
		Search.nodes = 0;
		Search.pv.clear();
		Search.ply = ply;

		auto t1 = Clock::now();
		negamaxRoot(Search, ply);
		auto t2 = Clock::now();

		std::chrono::duration<float, std::milli> time = t2 - t1;

		if (Search.bestMove) {
			
			if (Search.bestScore == MATE || Search.bestScore == -MATE)
				Search.flagMate = true;

			std::cout << "\n*depth:" << ply << " nodes:" << Search.nodes << " ms:" << int(time.count()) << " nps:" 
				<< int(Search.nodes / (time.count() / 1000)) << std::endl;

			std::cout << "\t move:" << displayMove(Search.pos, Search.bestMove) << " score:";

			float score = static_cast<float>(Search.bestScore / 100.00);

			switch (Search.pos.getTurn()) {
			case WHITE: 
				if (score > 0)
					std::cout << "+";
				else if (score < 0)
					std::cout << "-";
				break;
			case BLACK:
				if (score > 0)
					std::cout << "-";
				else if (score < 0)
					std::cout << "+";
				break; 
			}
			
			(Search.flagMate == true) ? std::cout << "#" << (ply / 2) : std::cout << std::fixed << std::setprecision(2) << fabs(score) << " pv:";

			for (auto it = Search.pv.begin(); it != Search.pv.end(); ++it)
				(it == std::prev(Search.pv.end()) && (Search.bestScore == MATE || Search.bestScore == -MATE)) ? std::cout 
					<< displayMove(Search.pos, *it) << "# " : std::cout << displayMove(Search.pos, *it) << " ";
		}

		else if (!Search.bestMove && !underCheck(Search.pos.getTurn(), Search.pos)) {
			std::cout << "\nIt's STALEMATE!" << std::endl;
			Search.flagMate = true;
		}

		else if (!Search.bestMove && underCheck(Search.pos.getTurn(), Search.pos)) {
			p.setCheckmate(true);
			std::cout << "\nIt's CHECKMATE!" << std::endl;
		}
	}
	
	return Search.bestMove;
}

void negamaxRoot(Search& Search, ushort const depth) 
{
	Move bestMove{};
	TTEntry TTEntry{};

/*	if (TT::table[Search.pos.getZobrist() % TT_SIZE].key == uint32_t(Search.pos.getZobrist())) 
		TTEntry = TT::table[Search.pos.getZobrist() % TT_SIZE];

	if (TTEntry.depth >= Search.depth) {
		Search.bestScore = TTEntry.score;
		Search.bestMove = TTEntry.move;
	}

	else {  // position is not in TT */
		Search.bestScore = -MATE;
		std::vector<Move> moveList = moveGeneration(Search.pos);
		moveList = pruneIllegal(moveList, Search.pos);

		/* if (bestSoFar) {
			std::vector<Move>::iterator it = std::find(moveList.begin(), moveList.end(), bestSoFar);
			std::rotate(moveList.begin(), it, it + 1);
		} */

		for (const auto& m : moveList) {
			std::vector<Move> childPv{};
			Position copy = Search.pos;
			short score{};
			doMove(m, copy);
			score = -negamaxAB(copy, depth - 1, -BETA, -ALPHA, Search.nodes, childPv);

			if (score >= Search.bestScore) {
				Search.bestScore = score;
				Search.bestMove = m;
				Search.pv.clear();
				Search.pv.push_back(m);
				std::copy(childPv.begin(), childPv.end(), back_inserter(Search.pv));
			}

			undoMove(m, copy, Search.pos);
			Search.nodes++;
		}
}


const short negamaxAB(Position const& p, ushort const nodeDepth, short alpha, short beta, unsigned long& nodes, std::vector<Move> &childPv)
{
	Position copy = p;

	if (nodeDepth == 0)
		return quiescence(copy, alpha, beta, nodes);
		// return evaluate(p);

	short bestScore = -MATE;	
	Move bestMove{};
	std::vector<Move> moveList = moveGeneration(copy);
	moveList = pruneIllegal(moveList, copy);
	
	for (const auto& m : moveList) {
		short score{};
		std::vector<Move> nephewPv{};
		doMove(m, copy);
		score = -negamaxAB(copy, nodeDepth - 1, -beta, -alpha, nodes, nephewPv);
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