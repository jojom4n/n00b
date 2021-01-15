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
struct Search mySearch{};

const Move iterativeSearch(Position& p, short const& depth)
{
	mySearch.pos = p;
	unsigned int totalTime{};

	std::vector<Move> moves = moveGeneration(mySearch.pos);
	moves = pruneIllegal(moves, mySearch.pos);
	
	if (moves.size() == 0 && !underCheck(mySearch.pos.getTurn(), mySearch.pos)) {
		std::cout << "\nIt's STALEMATE!" << std::endl;
		return 0;
	}

	if ((moves.size() == 0 && underCheck(mySearch.pos.getTurn(), mySearch.pos))) {
		mySearch.pos.setCheckmate(true); // just in case position is examined for the first time
		std::cout << "\nIt's CHECKMATE!" << std::endl;
		return 0;
	}

	else {

		for (short ply = 1; ply <= depth; ply++) {
			mySearch.nodes = 0;
			mySearch.depth = ply;
			mySearch.ttHits = 0;
			mySearch.ttUseful = 0;

			auto depthTimeStart = Clock::now();
			mySearch.bestScore = negamaxAB<false>(mySearch.pos, ply, ALPHA, BETA, mySearch.pv);
			// mySearch.bestScore = pvs<false>(mySearch.pos, ply, ALPHA, BETA, mySearch.pv);
			auto depthTimeEnd = Clock::now();

			std::chrono::duration<float, std::milli> depthTime = depthTimeEnd - depthTimeStart;
			totalTime += static_cast<int>(depthTime.count());

			if (mySearch.bestMove) {

				std::cout << "\n*depth:" << ply << " nodes:" << mySearch.nodes << " ms:" << unsigned int(depthTime.count()) <<
					" total_ms:" << totalTime << " nps:" << unsigned int(mySearch.nodes / (depthTime.count() / 1000))
					<< " TT_hits:" << mySearch.ttHits << " TT_useful:" << mySearch.ttUseful << std::endl;

				std::cout << "\t move:" << displayMove(mySearch.pos, mySearch.bestMove) << " score:";
				
				if (!(mySearch.bestScore == MATE)) {
					
					float score = static_cast<float>(mySearch.bestScore / 100.00);
					
					switch (mySearch.pos.getTurn()) {
					case WHITE:
						if (score > 0.0f)
							std::cout << "+";
						else if (score < 0.0f)
							std::cout << "-";
							break;
					case BLACK:
						if (score > 0.0f)
							std::cout << "-";
						else if (score < 0.0f)
							std::cout << "+";
						break;
					default:
						break;
					}
					
					std::cout << std::setprecision(3) << fabs(score);
				}
				else {
					ushort flag{};
					for (flag = 1; mySearch.pv[flag] != 0; flag++);
					std::cout << "#" << (flag / 2);
				}

				std::cout << " pv:";

				for (ushort i = 0; mySearch.pv[i] != 0; i++)
				{
					std::cout << displayMove(mySearch.pos, mySearch.pv[i]) << " ";
				}
			}

			else if (!underCheck(mySearch.pos.getTurn(), mySearch.pos)) {
				std::cout << "\nIt's STALEMATE!" << std::endl;
				return 0;
			}

			else if (underCheck(mySearch.pos.getTurn(), mySearch.pos)) {
				std::cout << "\nIt's CHECKMATE!" << std::endl;
				mySearch.pos.setCheckmate(true);
				return 0;
			}
		}

		return mySearch.bestMove;
	}
}


template<bool nullMove>
const short negamaxAB(Position const& p, short const& depth, short alpha, short beta, Move* pv)
{
	Move bestMove{}, subPV[64]{};
	short bestScore = -SHRT_INFINITY;
	
	if (depth == 0)
		return quiescence(p, alpha, beta);

	
	/* ********************************************************* */
	/*															 */
	/*  				  FUTILITY PRUNING                       */
	/*                                                           */
	/* ********************************************************* */
	if (!underCheck(p.getTurn(), p) && !p.isEnding()) {
		if ((depth == 1) && (lazyEval(p) + MARGIN < alpha))
			return quiescence(p, alpha, beta);
	}
	/* ********************************************************* */
	/*  				  END FUTILITY PRUNING                   */
	/* ********************************************************* */

	std::vector<Move> moveList = moveGeneration(p);
	moveList = pruneIllegal(moveList, p);

	// at PV node, first search best move from previous iteration
	if (std::find(moveList.begin(), moveList.end(), mySearch.bestMove) != moveList.end()) {
		std::vector<Move>::iterator it = std::find(moveList.begin(), moveList.end(), mySearch.bestMove);
		std::rotate(moveList.begin(), it, it + 1);
	}

	if (moveList.size() == 0 && underCheck(p.getTurn(), p))
		return -MATE;
	else if (moveList.size() == 0 && !underCheck(p.getTurn(), p))
		return 0;
	

	for (const auto& m : moveList) {
		Position copy = p;
		doMove(m, copy);
		mySearch.nodes++;
		short score = -negamaxAB<false>(copy, depth - 1, -beta, -alpha, subPV);
		undoMove(m, copy, p);

		if (score >= bestScore) {
			bestScore = score;
			bestMove = m;

			if (score > alpha)
				alpha = score;
		}

		if (score >= beta || score == MATE)
			break;
	}
	mySearch.bestMove = bestMove;
	return bestScore;
}


template<bool nullMove>
const short pvs(Position const& p, short const& depth, short alpha, short beta, Move* pv)
{
	Position copy = p;
	Move subPV[MAX_PLY]{};

	if (depth == 0)
		return quiescence(copy, alpha, beta);

	std::vector<Move> moveList = moveGeneration(copy);
	moveList = pruneIllegal(moveList, copy);
	// moveList = ordering(moves);


	if (moveList.size() > 0) {
		doMove(moveList[0], copy);
		mySearch.nodes++;
		short bestScore = -pvs<false>(copy, depth - 1, -beta, -alpha, subPV);
		undoMove(moveList[0], copy, p);
		mySearch.bestMove = moveList[0];
		pv[0] = mySearch.bestMove;
		memcpy(pv + 1, subPV, 63 * sizeof(Move));
		pv[63] = 0;
		moveList.erase(moveList.begin());

		for (const auto& m : moveList) {
			doMove(m, copy);
			mySearch.nodes++;
			short score = -pvs<false>(copy, depth - 1, -alpha - 1, -alpha, subPV);

			if (score > alpha && score < beta)
				score = -pvs<false>(copy, depth - 1, -beta, -alpha, subPV);

			undoMove(m, copy, p);

			if (score >= bestScore) {
				bestScore = score;
				mySearch.bestMove = m;
				pv[0] = mySearch.bestMove;
				memcpy(pv + 1, subPV, 63 * sizeof(Move));
				pv[63] = 0;
				if (score >= alpha) alpha = score;
				if (score >= beta) break;
			}
		}

		return bestScore;
	}
}


const short quiescence(Position const& p, short alpha, short beta)
{
	short stand_pat = lazyEval(p);
	
	if (stand_pat >= beta)
		return beta;

	if (alpha < stand_pat)
		alpha = stand_pat;

	std::vector<Move> moveList = moveGenQS(p);
	moveList = pruneIllegal(moveList, p);
	moveList = ordering(moveList);

	for (const auto& m : moveList) {
		Position copy = p;
		doMove(m, copy);
		mySearch.nodes++;
		short score = -quiescence(copy, -beta, -alpha);
		undoMove(m, copy, p);

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}