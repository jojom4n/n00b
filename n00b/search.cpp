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
struct Search mySearch({});

const Move iterativeSearch (Position &p, ushort const& depth)
{
	mySearch.pos = p;
	mySearch.flagMate = 0;
	unsigned int totalTime{};
			
	for (short ply = 1; ply <= depth && !mySearch.flagMate; ply++) {
		mySearch.nodes = 0;
		mySearch.ttHits = 0;
		mySearch.ttUseful = 0;
		mySearch.pv.clear();

		auto depthTimeStart = Clock::now();
		negamaxRoot(mySearch, ply);
		auto depthTimeEnd = Clock::now();

		std::chrono::duration<float, std::milli> depthTime = depthTimeEnd - depthTimeStart;
		totalTime += static_cast<int>(depthTime.count());

		if (mySearch.bestMove) {
			
			if (mySearch.bestScore == MATE || mySearch.bestScore == -MATE)
				mySearch.flagMate = true;

			std::cout << "\n*depth:" << ply << " nodes:" << mySearch.nodes << " ms:" << unsigned int(depthTime.count()) << 
				" total_ms:" << totalTime << " nps:" << unsigned int(mySearch.nodes / (depthTime.count() / 1000)) 
				<< " TT_hits:" << mySearch.ttHits << " TT_useful:" << mySearch.ttUseful << std::endl; 

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

			for (const auto& elem : mySearch.pv)
				std::cout << elem << " ";
			
			/* for (auto it = mySearch.pv.begin(); it != mySearch.pv.end(); ++it)
				(it == std::prev(mySearch.pv.end()) && (mySearch.bestScore == MATE || mySearch.bestScore == -MATE)) ? std::cout 
					<< displayMove(mySearch.pos, *it) << "# " : std::cout << displayMove(mySearch.pos, *it) << " "; */
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
	std::vector<Move> moves = moveGeneration(mySearch.pos), moveList{};
	moves = pruneIllegal(moves, mySearch.pos);
	moveList = ordering(moves);

	for (const auto& m : moveList) {
		Position copy = mySearch.pos;
		std::list<std::string> childPv;
		short score{};
		doMove(m, copy);
		score = -negamaxAB(copy, depth - 1, -BETA, -ALPHA, mySearch.nodes, childPv);

		if (score >= mySearch.bestScore) {
			mySearch.bestScore = score;
			mySearch.bestMove = m;
			mySearch.pv.clear();
			mySearch.pv.push_back(displayMove(mySearch.pos, m));
			std::copy(childPv.begin(), childPv.end(), back_inserter(mySearch.pv));
		}

		undoMove(m, copy, mySearch.pos);
		mySearch.nodes++;
	}
}


const short negamaxAB(Position const& p, ushort const& depth, short alpha, short beta, unsigned long long& nodes, std::list<std::string>& childPv)
{
	Position copy = p;
	Move bestMove{};
	short bestScore = -MATE, alphaOrig = alpha;
	uint32_t key = static_cast<uint32_t>(copy.getZobrist());
	TTEntry TTEntry{};
	
	if (TT::table[key % TT_SIZE].key == key) {
		TTEntry = TT::table[key % TT_SIZE];
		mySearch.ttHits++;
		childPv.push_back(displayMove(copy, TTEntry.move));

		if (TT::isLegalEntry(TTEntry, copy) && TTEntry.depth >= depth) {

			mySearch.ttUseful++;

			switch (TTEntry.nodeType) {
			case EXACT:
				childPv.push_back(displayMove(copy, TTEntry.move)); // PROBLEM WITH TRUNCATED PV IF WE JUMP ALL THE OTHER DEPTH SEARCHES
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

			if (alpha >= beta) {
				childPv.push_back(displayMove(copy, TTEntry.move));
				return TTEntry.score;
			}
		}
	} 
	
	if (depth == 0)
		return quiescence(copy, alpha, beta, nodes);
		// return evaluate(p);
	
	// position is not in TT, or we received only upper- or lower-bound values
	std::vector<Move> moves = moveGeneration(copy), moveList{};
	moves = pruneIllegal(moves, copy);
	moveList = ordering(moves);

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
		std::list<std::string> nephewPv;
		doMove(m, copy);
		score = -negamaxAB(copy, depth - 1, -beta, -alpha, nodes, nephewPv);
		undoMove(m, copy, p);
		nodes++;

		if (score >= beta) {
			bestScore = score;
			bestMove = m;
			childPv.clear();
			childPv.push_back(displayMove(copy, bestMove));
			std::copy(nephewPv.begin(), nephewPv.end(), back_inserter(childPv));
			break;
		}	
		
		if (score > bestScore) {
			bestScore = score;
			bestMove = m;
			
			if (score > alpha)
				alpha = score;

			childPv.clear();
			childPv.push_back(displayMove(copy, bestMove));
			std::copy(nephewPv.begin(), nephewPv.end(), back_inserter(childPv));	
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
	std::vector<Move> moves = moveGenQS(copy), moveList{};
	moves = pruneIllegal(moves, copy);
	moveList = ordering(moves);

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