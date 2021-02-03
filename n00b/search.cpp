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

extern std::vector<TTEntry> TT::table;
struct Search mySearch{};

const Move iterativeSearch(Position& p, short const& depth)
{
	mySearch.pos = p;
	unsigned int totalTime{};

	std::vector<Move> moveList;
	moveList.reserve(MAX_PLY);
	moveList = moveGeneration(mySearch.pos);
	pruneIllegal(moveList, mySearch.pos);
	
	if (moveList.size() == 0) {
		
		if (!underCheck(mySearch.pos.getTurn(), mySearch.pos))
			std::cout << "\nIt's STALEMATE!" << std::endl;
		else if (underCheck(mySearch.pos.getTurn(), mySearch.pos)) {
			mySearch.pos.setCheckmate(true); // just in case position is examined for the first time
			std::cout << "\nIt's CHECKMATE!" << std::endl;
		}
		
		return 0;
	}

	else {

		for (short ply = 1; ply <= depth; ply++) {
			mySearch.nodes = 0;
			mySearch.depth = ply;
			mySearch.ttHits = 0;
			mySearch.ttUseful = 0;

			auto depthTimeStart = Clock::now();
			mySearch.bestScore = pvs(mySearch.pos, ply, ALPHA, BETA, mySearch.pv);
			auto depthTimeEnd = Clock::now();

			std::chrono::duration<float, std::milli> depthTime = depthTimeEnd - depthTimeStart;
			totalTime += static_cast<int>(depthTime.count());

			if (mySearch.bestMove) {

				std::cout << "\n*depth:" << ply << " nodes:" << mySearch.nodes << " ms:" << unsigned int(depthTime.count()) <<
					" total_ms:" << totalTime << " nps:" << unsigned int(mySearch.nodes / (depthTime.count() / 1000))
					<< " TT_hits:" << mySearch.ttHits << " TT_useful:" << mySearch.ttUseful << std::endl;

				std::cout << "\t move:" << displayMove(mySearch.pos, mySearch.bestMove) << " score:";
				
				if (!(mySearch.bestScore == (MATE + (depth - ply))) && !(mySearch.bestScore == -MATE - (depth - ply))) {
					
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


//template<bool nullMove>
//const short negamaxAB(Position const& p, short const& depth, short alpha, short beta, Move* pv)
//{
//	Move bestMove{}, subPV[64]{};
//	short bestScore = -SHRT_INFINITY;
//	
//	if (depth == 0)
//		return quiescence(p, ALPHA, BETA);
//
//
//	std::vector<Move> moveList = moveGeneration(p);
//	moveList = pruneIllegal(moveList, p);
//
//	// at PV node, first search best move from previous iteration
//	if (std::find(moveList.begin(), moveList.end(), mySearch.bestMove) != moveList.end()) {
//		std::vector<Move>::iterator it = std::find(moveList.begin(), moveList.end(), mySearch.bestMove);
//		std::rotate(moveList.begin(), it, it + 1);
//	}
//
//	if (moveList.size() == 0 && underCheck(p.getTurn(), p))
//		return -MATE;
//	else if (moveList.size() == 0 && !underCheck(p.getTurn(), p))
//		return 0;
//	
//
//	for (const auto& m : moveList) {
//		Position copy = p;
//		doMove(m, copy);
//		mySearch.nodes++;
//		short score = -negamaxAB<false>(copy, depth - 1, -beta, -alpha, subPV);
//		undoMove(m, copy, p);
//
//		if (score >= bestScore) {
//			bestScore = score;
//			bestMove = m;
//
//			if (score > alpha)
//				alpha = score;
//		}
//
//		if (score >= beta || score == MATE)
//			break;
//	}
//	mySearch.bestMove = bestMove;
//	return bestScore;
//}


const short pvs(Position& p, short const& depth, short alpha, short beta, Move* pv)
{	
	pv[0] = 0;
	uint32_t key = static_cast<uint32_t>(p.getZobrist());
	auto alphaOrig = alpha;
	TTEntry TTEntry{};
	

	/* ********************************************************* */
	/*                                                           */
	/*                  TRANSPOSITION TABLE                      */
	/*                                                           */
	/* ********************************************************* */
	if (TT::table[key % TT_SIZE].key == key) {
		TTEntry = TT::table[key % TT_SIZE];
		mySearch.ttHits++;

		if (TT::isLegalEntry(TTEntry, p) && TTEntry.depth >= depth) {

			mySearch.ttUseful++;

			switch (TTEntry.nodeType) {
			case LOWER_BOUND:
				if (alpha < TTEntry.score)
					alpha = TTEntry.score;
				break;
			case UPPER_BOUND:
				if (beta > TTEntry.score)
					beta = TTEntry.score;
				break;
			}
		}
	}
	
	
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


	Move bestMove{}, subPV[MAX_PLY]{};
	std::vector<Move> moveList;
	moveList.reserve(MAX_PLY);
	moveList = moveGeneration(p);
	moveList = ordering(moveList, p);

	if (std::find(moveList.begin(), moveList.end(), mySearch.bestMove) != moveList.end()) {
		std::vector<Move>::iterator it = std::find(moveList.begin(), moveList.end(), mySearch.bestMove);
		std::rotate(moveList.begin(), it, it + 1); // PERFORMANCE CRITICAL. TODO CHANGE
	}

	p.storeState(depth);
	ushort idx, legalMoves = ushort(moveList.size());

	for (idx = 0; idx < moveList.size(); idx++, legalMoves--) {
		Color c = Color(((C64(1) << 1) - 1) & (moveList[idx] >> 12)); // who is going to move?
		doMove(moveList[idx], p);

		if (!underCheck(c, p))   // move is legal, let's go out of the loop
			break;
		
		undoMove(moveList[idx], p);  // move is illegal, let's restore position and try next one
		p.restoreState(depth);
	}
	
	if (legalMoves == 0) {
		ushort check = underCheck(p.getTurn(), p);
		if (check)
			return -(MATE + depth);
		else if (!check)
			return 0;
	}

	mySearch.nodes++;
	short bestScore = -pvs(p, depth - 1, -beta, -alpha, subPV);
	undoMove(moveList[idx], p);
	p.restoreState(depth);
	

	if (bestScore > alpha) {
		bestMove = moveList[idx];
		pv[0] = bestMove;
		memcpy(pv + 1, subPV, 63 * sizeof(Move));
		pv[63] = 0;
		
		if (bestScore >= beta)			
			return bestScore;
		
		alpha = bestScore;
	}

	for (ushort i = idx + 1; i < moveList.size(); i++)
	{
		short score{};
		p.storeState(depth);
		Color c = Color(((C64(1) << 1) - 1) & (moveList[i] >> 12)); // who is going to move?
		doMove(moveList[i], p);

		if (underCheck(c, p)) {  // move is not legal, let's continue to search
			undoMove(moveList[i], p);
			p.restoreState(depth);
			goto OUTER;
		}

		mySearch.nodes++;
		score = -pvs(p, depth - 1, -alpha - 1, -alpha, subPV);


		if (score > alpha && score < beta) {
			score = -pvs(p, depth - 1, -beta, -alpha, subPV);

			if (score > alpha)
				alpha = score;
		}

		undoMove(moveList[i], p);
		p.restoreState(depth);

		if (score > bestScore) {

			if (score >= beta)
				return score;
			
			bestMove = moveList[i];
			bestScore = score;
			pv[0] = bestMove;
			memcpy(pv + 1, subPV, 63 * sizeof(Move));
			pv[63] = 0;
		}

	OUTER: continue;

	}


	/* ********************************************************* */
	/*                 UPDATE TRANSPOSITION TABLE                */
	/*                                                           */
	/* we only update TT if there is a best move.In other words, */
	/* if it's mate and therefore there is not any best move (or */
	/* hence any move at all), we do not want to update TT,      */
	/* because otherwise we would have a dummy entry with score  */
	/* about MATE or -MATE and move = 0, polluting TT!           */
	/*                                                           */
	/* ********************************************************* */
	if (bestMove) {
		TTEntry.score = bestScore;
		if (bestScore <= alphaOrig)
			TTEntry.nodeType = UPPER_BOUND;
		else if (bestScore >= beta)
			TTEntry.nodeType = LOWER_BOUND;
		else
			TTEntry.nodeType = EXACT;

		TTEntry.age = static_cast<uint8_t>(p.getMoveNumber());
		TTEntry.depth = static_cast<uint8_t>(depth);
		TTEntry.key = static_cast<uint32_t>(p.getZobrist());
		TTEntry.move = bestMove;
		TT::Store(TTEntry);
	}

	mySearch.bestMove = bestMove;
	return bestScore;
}


const short quiescence(Position p, short alpha, short beta, ushort qsDepth)
{
	uint32_t key = static_cast<uint32_t>(p.getZobrist());
	TTEntry TTEntry;
	short stand_pat = lazyEval(p);
	
	if (qsDepth <= 0)
		return stand_pat;

	if (stand_pat >= beta)
		return beta;

	if (alpha < stand_pat)
		alpha = stand_pat;

	std::vector<Move> moveList;
	moveList.reserve(MAX_PLY);
	moveList = moveGenQS(p);
	pruneIllegal(moveList, p);
	
	if (moveList.size() > 0)
		moveList = mvv_lva(moveList);

	for (const auto& m : moveList) {
		p.storeState(qsDepth);
		doMove(m, p);
		mySearch.nodes++;
		short score = -quiescence(p, -beta, -alpha, qsDepth -1);
		undoMove(m, p);
		p.restoreState(qsDepth);

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}