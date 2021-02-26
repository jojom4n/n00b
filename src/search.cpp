#include "pch.h"
#include "search.h"
#include "display.h"
#include "evaluation.h"
#include "makemove.h"
#include "movegen.h"
#include "moveorder.h"
#include <cmath>
#include <iomanip>
#include <iostream>

struct Search mySearch{};

const Move iterativeSearch(Position& p, short const& depth)
{
	mySearch.pos = p;
	mySearch.depth = depth;
	
	// let's reset history table	
	std::fill(&mySearch.historyTbl[0][0][0],
			  &mySearch.historyTbl[0][0][0] + sizeof(mySearch.historyTbl), 
			  0);
	
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
			mySearch.ttHits = 0;
			mySearch.ttUseful = 0;

			auto depthTimeStart = Clock::now();
			mySearch.bestScore = pvs<false>(mySearch.pos, ply, ALPHA, BETA, mySearch.pv);
			auto depthTimeEnd = Clock::now();

			std::chrono::duration<float, std::milli> depthTime = depthTimeEnd - depthTimeStart;
			totalTime += static_cast<int>(depthTime.count());

			if (mySearch.bestMove) {

				std::cout << "*depth:" << ply << " nodes:" << mySearch.nodes << " ms:" << (unsigned int)(depthTime.count()) <<
					" total_ms:" << totalTime << " nps:" << (unsigned int)(mySearch.nodes / (depthTime.count() / 1000))
					<< " TT_hits:" << mySearch.ttHits << " TT_useful:" << mySearch.ttUseful << "\n";

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

				std::cout << std::endl;
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


template <bool nullMove>
const short pvs(Position& p, short depth, short alpha, short beta, Move* pv)
{
	pv[0] = 0;
		
	if (depth <= 0)
		return quiescence(p, alpha, beta);
		
	/* ********************************************************* */
	/*  				  EXTENDED NULL MOVE PRUNING             */
	/* ********************************************************* */
	if (nullMove && !underCheck(p.getTurn(), p)) {
		// const ushort R_Null = determineR(depth, p);
		const ushort R_Null = depth > 6 ? MAX_R : MIN_R;
		doNullMove(depth, p);
		short nullScore = -pvs<false>(p, depth - 1 - R_Null, -beta, -beta + 1, pv);
		p.restoreState(depth); // undo Null Move

		if (nullScore >= beta) {
			// return nullScore;
			depth -= DR;

			if (depth <= 0)
				return quiescence(p, alpha, beta);
		} 
	}


	/* ********************************************************* */
	/*  				  FUTILITY PRUNING                       */
	/* ********************************************************* */
	if (depth == 1)
		if (lazyEval(p) + MARGIN < alpha)
			if (!underCheck(p.getTurn(), p) && !p.isEnding())
				return quiescence(p, alpha, beta);
	
	
	/* ********************************************************* */
	/*  				 EXTENDED FUTILITY PRUNING               */
	/* ********************************************************* */
	if (depth == 2)
		if (lazyEval(p) + EXTENDED_MARGIN < alpha)
			if (!underCheck(p.getTurn(), p) && !p.isEnding())
				return quiescence(p, alpha, beta);



	Move bestMove{}, subPV[MAX_PLY]{};
	std::vector<Move> moveList;
	moveList.reserve(MAX_PLY);
	moveList = moveGeneration(p);
	moveList = ordering(moveList, p, depth);
	p.storeState(depth);
	ushort idx, legalMoves = ushort(moveList.size());

	for (idx = 0; idx < moveList.size(); idx++, legalMoves--) {
		
		if (doMove(moveList[idx], p)) // we have a legal PV node
			break;
		
		undoMove(moveList[idx], p);  // move is illegal, let's restore position and try next one
		p.restoreState(depth);
	}
	
	if (legalMoves == 0) {
		if (underCheck(p.getTurn(), p))
			return -(MATE + depth);
		else
			return 0;
	}

	mySearch.nodes++;
	short bestScore = -pvs<false>(p, depth - 1, -beta, -alpha, subPV);
	undoMove(moveList[idx], p);
	p.restoreState(depth);
	updateHistoryTBL(depth, moveList[idx], beta, bestScore);
		
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
		ushort killerIndex{};
		p.storeState(depth);
		
		if (doMove(moveList[i], p) == false) { // move is not legal
			undoMove(moveList[i], p);
			p.restoreState(depth);
			continue; // since move is not legal, let's skip this move and go to next iteration of for() loop
		}
		
		mySearch.nodes++;
		score = -pvs<true>(p, depth - 1, -alpha - 1, -alpha, subPV);

		if (score > alpha && score < beta) {
			score = -pvs<true>(p, depth - 1, -beta, -alpha, subPV);

			if (score > alpha)
				alpha = score;
		}

		undoMove(moveList[i], p);
		p.restoreState(depth);
		updateHistoryTBL(depth, moveList[i], beta, score);
		
		if (score > bestScore) {

			if (score >= beta) {
			
				if (mySearch.killerMoves[depth].size() < 2) {
					mySearch.killerMoves[depth][killerIndex] = moveList[i];
					killerIndex++;
				}
				
				return score;
			}
			
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


const short quiescence(Position p, short alpha, short beta)
{
	short stand_pat = lazyEval(p);
	
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
		p.storeState();
		doMove(m, p);
		mySearch.nodes++;
		short score = -quiescence(p, -beta, -alpha);
		undoMove(m, p);
		p.restoreState();

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}


void updateHistoryTBL(short const& depth, Move const& m, short const& beta, short const& score)
{
	ushort const moveType = ((C64(1) << 3) - 1) & (m >> 6);
	ushort const piece = Piece(((C64(1) << 3) - 1) & (m >> 9));
	ushort const squareTo = Square(((C64(1) << 6) - 1) & (m >> 13));
	ushort color = Color(((C64(1) << 1) - 1) & (m >> 12));
	
	if (depth <= 8 && !(moveType == CAPTURE)) {
		if (score >= beta)
			mySearch.historyTbl[color][piece][squareTo] += depth * depth;
		else
			mySearch.historyTbl[color][piece][squareTo] -= depth;

		if (abs(mySearch.historyTbl[color][piece][squareTo] >= 2000))
			mySearch.historyTbl[color][piece][squareTo] /= 2;
	}
}


/* const ushort determineR(short const& depth, Position const& p)
{
	ushort deltaDepth = mySearch.depth - depth;
	ushort piecesNo = p.count(WHITE) + p.count(BLACK);

	if ((deltaDepth <= 6) || ((deltaDepth <= 8) && (piecesNo < 3)))
		return MIN_R;
	else if ((deltaDepth > 8) || ((deltaDepth >= 6) && (piecesNo >= 3)))
		return MAX_R;
	else
		return MIN_R;
} */
