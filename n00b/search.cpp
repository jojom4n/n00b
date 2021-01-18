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

	std::vector<Move> moveList = moveGeneration(mySearch.pos);
	moveList = pruneIllegal(moveList, mySearch.pos);
	
	if (moveList.size() == 0 && !underCheck(mySearch.pos.getTurn(), mySearch.pos)) {
		std::cout << "\nIt's STALEMATE!" << std::endl;
		return 0;
	}

	if ((moveList.size() == 0 && underCheck(mySearch.pos.getTurn(), mySearch.pos))) {
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
			// mySearch.bestScore = negamaxAB<false>(mySearch.pos, ply, ALPHA, BETA, mySearch.pv);
			mySearch.bestScore = pvs(mySearch.pos, ply, ALPHA, BETA, mySearch.pv);
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
					ushort index{};
					for (index = 0; mySearch.pv[index] != 0; index++);
					std::cout << "#" << (index / 2 + 1);
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
		return quiescence(p, ALPHA, BETA);


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


const short pvs(Position const& p, short const& depth, short alpha, short beta, Move* pv)
{
	Move bestMove{}, subPV[MAX_PLY]{};
	std::vector<Move> moveList = moveGeneration(p);
	moveList = pruneIllegal(moveList, p);
	pv[0] = 0;

	if (moveList.size() == 0)
		if (underCheck(p.getTurn(), p))
			return -MATE;
		else if (!underCheck(p.getTurn(), p))
			return 0;

	if (depth <= 0)
		return quiescence(p, alpha, beta);


	/* ********************************************************* */
	/*															 */
	/*  				  FUTILITY PRUNING                       */
	/*                                                           */
	/* ********************************************************* */
	if (depth == 1)
		if (lazyEval(p) + MARGIN < alpha)
			if (!underCheck(p.getTurn(), p) && !p.isEnding())
				return quiescence(p, alpha, beta);
	/* ********************************************************* */
	/*  				END FUTILITY PRUNING                     */
	/* ********************************************************* */


	moveList = ordering(moveList, p);
	Position copy = p;
	doMove(moveList[0], copy);
	mySearch.nodes++;
	short bestScore = -pvs(copy, depth - 1, -beta, -alpha, subPV);
	undoMove(moveList[0], copy, p);

	if (bestScore > alpha) {
		bestMove = moveList[0];
		pv[0] = bestMove;
		memcpy(pv + 1, subPV, 63 * sizeof(Move));
		pv[63] = 0;
		
		if (bestScore >= beta)
			return bestScore;
		
		alpha = bestScore;
	}

	for (ushort i = 1; i < moveList.size(); i++)
	{
		doMove(moveList[i], copy);
		mySearch.nodes++;
		short score = -pvs(copy, depth - 1, -alpha - 1, -alpha, subPV);


		if (score > alpha && score < beta) {
			score = -pvs(copy, depth - 1, -beta, -alpha, subPV);

			if (score > alpha)
				alpha = score;
		}

		undoMove(moveList[i], copy, p);

		if (score > bestScore) {
			if (score >= beta)
				return score;
			
			bestMove = moveList[i];
			bestScore = score;
			pv[0] = bestMove;
			memcpy(pv + 1, subPV, 63 * sizeof(Move));
			pv[63] = 0;
		}
	}
		
	mySearch.bestMove = bestMove;
	return bestScore;
}


const short quiescence(Position const& p, short alpha, short beta, ushort qsDepth)
{
	if (qsDepth <= 0)
		return lazyEval(p);

	short stand_pat = lazyEval(p);

	if (stand_pat >= beta)
		return beta;

	if (alpha < stand_pat)
		alpha = stand_pat;

	std::vector<Move> moveList = moveGenQS(p);
	moveList = pruneIllegal(moveList, p);
	
	if (moveList.size() > 0)
		moveList = mvv_lva(moveList);

	for (const auto& m : moveList) {
		Position copy = p;
		doMove(m, copy);
		mySearch.nodes++;
		short score = -quiescence(copy, -beta, -alpha, qsDepth -1);
		undoMove(m, copy, p);

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}