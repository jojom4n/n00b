#include "pch.h"
#include "search.h"
#include "display.h"
#include "evaluation.h"
#include "makemove.h"
#include "movegen.h"
#include "moveorder.h"
#include "tt.h"
#include <iomanip>
#include <iostream>

extern std::array<TTEntry, TT_SIZE> TT::table;

const Move iterativeSearch (Position &p, ushort const& depth)
{
	struct Search mySearch({});
	mySearch.pos = p;
		
	for (short ply = 1; ply <= depth && !mySearch.flagMate; ply++) {
		mySearch.nodes = 0;
		mySearch.pv.clear();

		auto t1 = Clock::now();
		negamaxRoot(mySearch, ply);
		auto t2 = Clock::now();

		std::chrono::duration<float, std::milli> time = t2 - t1;

		if (mySearch.bestMove) {
			
			if (mySearch.bestScore == MATE || mySearch.bestScore == -MATE)
				mySearch.flagMate = true;

			std::cout << "\n*depth:" << ply << " nodes:" << mySearch.nodes << " ms:" << int(time.count()) << " nps:" 
				<< int(mySearch.nodes / (time.count() / 1000)) << std::endl;

			std::cout << "\t move:" << displayMove(mySearch.pos, mySearch.bestMove) << " score:";

			float score = static_cast<float>(mySearch.bestScore / 100.00);

			switch (mySearch.pos.getTurn()) {
			case WHITE: 
				if (score > 0.0f && mySearch.bestScore != MATE)
					std::cout << "+";
				else if (score < 0.0f)
					std::cout << "-";
				break;
			case BLACK:
				if (score > 0.0f)
					std::cout << "-";
				else if (score < 0.0f && mySearch.bestScore != MATE)
					std::cout << "+";
				break; 
			default:
				break;
			}
			
			(!mySearch.bestScore == 0 && mySearch.flagMate == true) ? std::cout << "#" << (ply / 2) 
				: std::cout << std::setprecision(3) << fabs(score);

			std::cout << " pv:";

			for (auto it = mySearch.pv.begin(); it != mySearch.pv.end(); ++it)
				(it == std::prev(mySearch.pv.end()) && (mySearch.bestScore == MATE || mySearch.bestScore == -MATE)) ? std::cout 
					<< displayMove(mySearch.pos, *it) << "# " : std::cout << displayMove(mySearch.pos, *it) << " ";
		}

		else if (!mySearch.bestMove && !underCheck(mySearch.pos.getTurn(), mySearch.pos)) {
			std::cout << "\nIt's STALEMATE!" << std::endl;
			mySearch.flagMate = true;
		}

		else if (!mySearch.bestMove && underCheck(mySearch.pos.getTurn(), mySearch.pos)) {
			p.setCheckmate(true);
			std::cout << "\nIt's CHECKMATE!" << std::endl;
		}
	}
	
	return mySearch.bestMove;
}

void negamaxRoot(struct Search& mySearch, ushort const& depth)
{
	mySearch.bestScore = -MATE;
	mySearch.bestMove = 0;
	std::vector<Move> moveList = moveGeneration(mySearch.pos);
	moveList = pruneIllegal(moveList, mySearch.pos);

	for (const auto& m : moveList) {
		std::vector<Move> childPv{};
		Position copy = mySearch.pos;
		short score{};
		doMove(m, copy);
		score = -negamaxAB(copy, depth - 1, -BETA, -ALPHA, mySearch.nodes, childPv);

		if (score >= mySearch.bestScore) {
			mySearch.bestScore = score;
			mySearch.bestMove = m;
			mySearch.pv.clear();
			mySearch.pv.push_back(m);
			std::copy(childPv.begin(), childPv.end(), back_inserter(mySearch.pv));
		}

		undoMove(m, copy, mySearch.pos);
		mySearch.nodes++;
	}
}


const short negamaxAB(Position const& p, ushort const& depth, short alpha, short beta, unsigned long long& nodes, std::vector<Move> &childPv)
{
	Position copy = p;
	Move bestMove{};
	short bestScore = -MATE, alphaOrig = alpha;
	uint32_t key = static_cast<uint32_t>(copy.getZobrist());
	TTEntry TTEntry{};
	
	if (TT::table[key % TT_SIZE].key == key) {
		TTEntry = TT::table[key % TT_SIZE];

		// TTENTRY LEGALITY MUST BE CHECKED **** PLACEHOLDER //

		if (TTEntry.depth >= depth) {

			switch (TTEntry.nodeType) {
			case EXACT:
				return TTEntry.score;
				break;
			case LOWER_BOUND:
				if (alpha < TTEntry.score)
					alpha = TTEntry.score;
				break;
			case UPPER_BOUND:
				if (beta > TTEntry.score)
					beta = TTEntry.score;
				break;
			}

			if (alpha >= beta)
				return TTEntry.score;
		}
	} 
	
	if (depth == 0)
		return quiescence(copy, alpha, beta, nodes);
		// return evaluate(p);
	
	// position is not in TT, or we received only upper- or lower-bound values
	std::vector<Move> moveList = moveGeneration(copy);
	moveList = pruneIllegal(moveList, copy);

	/* we found an hash move with minor depth than current one, so we cannot directly use it?
	Nonetheless, let's try the hash move first for our ordinary search  */
	if (TTEntry.move) {
		if (std::find(moveList.begin(), moveList.end(), TTEntry.move) != moveList.end()) {
			std::vector<Move>::iterator it = std::find(moveList.begin(), moveList.end(), TTEntry.move);
			std::rotate(moveList.begin(), it, it + 1);
		}
	}
	
	for (const auto& m : moveList) {
		short score{};
		std::vector<Move> nephewPv{};
		doMove(m, copy);
		score = -negamaxAB(copy, depth - 1, -beta, -alpha, nodes, nephewPv);
		undoMove(m, copy, p);
		nodes++;

		if (score >= beta) {
			bestScore = score;
			bestMove = m;
			break;
		}	
		
		if (score > bestScore) {
			bestScore = score;
			bestMove = m;

			if (score > alpha)
				alpha = score;
		}
	}

	TTEntry.score = bestScore;
	if (bestScore <= alphaOrig)
		TTEntry.nodeType = UPPER_BOUND;
	else if (bestScore >= beta)
		TTEntry.nodeType = LOWER_BOUND;
	else 
		TTEntry.nodeType = EXACT;
	
	TTEntry.age = static_cast<uint8_t>(copy.getMoveNumber());
	TTEntry.depth = static_cast<uint8_t>(depth);
	TTEntry.key = static_cast<uint32_t>(copy.getZobrist());
	TTEntry.move = bestMove;
	TT::Store(TTEntry); 
	
	return bestScore;
}


const short quiescence(Position const& p, short alpha, short beta, unsigned long long &nodes)
{
	short stand_pat = evaluate(p);

	if (stand_pat >= beta)
		return beta;

	if (alpha < stand_pat)
		alpha = stand_pat;

	Position copy = p;
	std::vector<Move> moveList = moveGenQS(copy);
	moveList = pruneIllegal(moveList, copy);
	moveList = mvv_lva(moveList);

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