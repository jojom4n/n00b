#include "pch.h"
#include "search.h"
#include "display.h"
#include "evaluation.h"
#include "makemove.h"
#include "movegen.h"
#include "moveorder.h"
#include "tt.h"
#include <algorithm>
#include <iomanip>
#include <iostream>

extern std::array<TTEntry, TT_SIZE> TT::table;
struct Search mySearch({});


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
			mySearch.bestScore = -SHRT_INFINITY;
			mySearch.ttHits = 0;
			mySearch.ttUseful = 0;

			auto depthTimeStart = Clock::now();
			// mySearch.bestScore = negamaxAB<false>(mySearch.pos, ply, ALPHA, BETA, mySearch.pv);
			mySearch.bestScore = pvs<false>(mySearch.pos, ply, ALPHA, BETA, mySearch.pv);
			auto depthTimeEnd = Clock::now();

			std::chrono::duration<float, std::milli> depthTime = depthTimeEnd - depthTimeStart;
			totalTime += static_cast<int>(depthTime.count());

			if (mySearch.bestMove) {

				std::cout << "\n*depth:" << ply << " nodes:" << mySearch.nodes << " ms:" << unsigned int(depthTime.count()) <<
					" total_ms:" << totalTime << " nps:" << unsigned int(mySearch.nodes / (depthTime.count() / 1000))
					<< " TT_hits:" << mySearch.ttHits << " TT_useful:" << mySearch.ttUseful << std::endl;

				std::cout << "\t move:" << displayMove(mySearch.pos, mySearch.bestMove) << " score:";
				
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

				mySearch.bestScore == MATE ? std::cout << "#" << (ply / 2)
					: std::cout << std::setprecision(3) << fabs(score);

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
const short pvs(Position const& p, short const& depth, short alpha, short beta, Move* pv)
{
	Position copy = p;
	Move bestMove{}, subPV[64]{};
	pv[0] = 0;
	short bestScore = -MATE + (mySearch.depth - depth), alphaOrig = alpha;
	uint32_t key = static_cast<uint32_t>(copy.getZobrist());
	TTEntry TTEntry{};

	
	if (TT::table[key % TT_SIZE].key == key && nullMove == false) {
		TTEntry = TT::table[key % TT_SIZE];
		mySearch.ttHits++;

		if (TT::isLegalEntry(TTEntry, copy) && TTEntry.depth >= depth) {

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
			default:
				break;
			}

			if (alpha >= beta) {
				return TTEntry.score;
			}
		}
	}


	if (depth <= 0)
		return quiescence(copy, alpha, beta);

	std::vector<Move> moves = moveGeneration(copy), moveList{};
	moves = pruneIllegal(moves, copy);
	moveList = ordering(moves);
	
	// null-move pruning. We only use it if side is not under check and if game
	// is not in ending state (to avoid zugzwang issues with null-move pruning)
	if (!underCheck(copy.getTurn(), copy) && !copy.isEnding()) {

		// let's do a dummy (null-) move, then proceed with pruning
		if (copy.getTurn() == BLACK)
			copy.setMoveNumber(copy.getMoveNumber() + 1);

		copy.updateZobrist(copy.getTurn());

		if (copy.getTurn() == WHITE)
			copy.setTurn(BLACK);
		else
			copy.setTurn(WHITE);

		copy.updateZobrist(copy.getTurn());

		if (copy.getEnPassant() != SQ_EMPTY) {
			copy.updateZobrist(copy.getEnPassant());
			copy.setEnPassant(SQ_EMPTY);
			copy.updateZobrist(copy.getEnPassant());
		}

		short score = -pvs<true>(copy, depth - R - 1, -beta, -beta + 1, subPV);

		if (score >= beta)
			return score;
	}
		
	
	/* we found an hash move with minor depth than current one, so we cannot directly use it?
	Nonetheless, let's try the hash move first for our ordinary search  */
	if (TTEntry.move) {
		if (std::find(moveList.begin(), moveList.end(), TTEntry.move) != moveList.end()) {
			std::vector<Move>::iterator it = std::find(moveList.begin(), moveList.end(), TTEntry.move);
			std::rotate(moveList.begin(), it, it + 1);
		}
	}
	
	if (moveList.size() > 1) {
		doMove(moveList[0], copy);
		mySearch.nodes++;
		bestMove = moveList[0];
		bestScore = -pvs<false>(copy, depth - 1, -beta, -alpha, subPV);
		undoMove(moveList[0], copy, p);

		if (bestScore > alpha) {
			if (bestScore >= beta)
				return bestScore;

			pv[0] = moveList[0];
			memcpy(pv + 1, subPV, 63 * sizeof(Move));
			pv[63] = 0;
			alpha = bestScore;
		}

		moveList.erase(moveList.begin());
	}
	
	for (const auto& m : moveList) {
		doMove(m, copy);
		mySearch.nodes++;
		
		short score = -pvs<false>(copy, depth - 1, -alpha - 1, -alpha, subPV);

		if ((score > alpha) && (score < beta)) {
			score = -pvs<false>(copy, depth - 1, -beta, -alpha, subPV);
			
			if (score > alpha) 
				alpha = score;
		}

		undoMove(m, copy, p);

		if (score > bestScore) {

			if (score >= beta) {
				bestScore = score;
				bestMove = m;
				break;
			}
			
			pv[0] = m;
			memcpy(pv + 1, subPV, 63 * sizeof(Move));
			pv[63] = 0;
			bestScore = score;
			bestMove = m;
		}
	}

	/* we only update TT if there is a best move. In other words, if it's mate and therefore
	there is not any best move (or, hence, any move at all), we do not want to update
	TT, because otherwise we would have a dummy entry with score MATE or -MATE and move = 0, polluting TT! */
	if (bestMove) {
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
	}

	mySearch.bestMove = bestMove;
	return bestScore;
}


template<bool nullMove>
const short negamaxAB(Position const& p, short const& depth, short alpha, short beta, Move* pv)
{
	Position copy = p;
	Move bestMove{}, subPV[64]{};
	pv[0] = 0;
	short bestScore = -MATE + (mySearch.depth - depth), alphaOrig = alpha;
	uint32_t key = static_cast<uint32_t>(copy.getZobrist());
	TTEntry TTEntry{};

	if (TT::table[key % TT_SIZE].key == key && nullMove == false) {
		TTEntry = TT::table[key % TT_SIZE];
		mySearch.ttHits++;

		if (TT::isLegalEntry(TTEntry, copy) && TTEntry.depth >= depth) {

			mySearch.ttUseful++;

			switch (TTEntry.nodeType) {
			/* case EXACT:
				return TTEntry.score;
				break; */
			case LOWER_BOUND:
				if (alpha < TTEntry.score)
					alpha = TTEntry.score;
				break;
			case UPPER_BOUND:
				if (beta > TTEntry.score)
					beta = TTEntry.score;
				break;
			default:
				break;
			}

			if (alpha >= beta) {
				return TTEntry.score;
			}
		}
	} 

	if (depth <= 0)
		return quiescence(copy, alpha, beta);
	
	// position is not in TT, or we received only upper- or lower-bound values
	std::vector<Move> moves = moveGeneration(copy), moveList{};
	moves = pruneIllegal(moves, copy);
	moveList = ordering(moves);

	// null-move pruning. We only use it if side is not under check and if game
	// is not in ending state (to avoid zugzwang issues with null-move pruning)
	if (!underCheck(copy.getTurn(), copy) && !copy.isEnding()) {

		// let's do a dummy (null-) move, then proceed with pruning
		if (copy.getTurn() == BLACK)
			copy.setMoveNumber(copy.getMoveNumber() + 1);

		copy.updateZobrist(copy.getTurn());

		if (copy.getTurn() == WHITE)
			copy.setTurn(BLACK);
		else
			copy.setTurn(WHITE);
		
		copy.updateZobrist(copy.getTurn());

		if (copy.getEnPassant() != SQ_EMPTY) {
			copy.updateZobrist(copy.getEnPassant());
			copy.setEnPassant(SQ_EMPTY);
			copy.updateZobrist(copy.getEnPassant());
		}

		short score = -negamaxAB<true>(copy, depth - R - 1, -beta, -beta + 1, subPV);

		if (score >= beta) 
			return score;
	}


	/* we found an hash move with minor depth than current one, so we cannot directly use it?
	Nonetheless, let's try the hash move first for our ordinary search  */
	if (TTEntry.move) {
		if (std::find(moveList.begin(), moveList.end(), TTEntry.move) != moveList.end()) {
			std::vector<Move>::iterator it = std::find(moveList.begin(), moveList.end(), TTEntry.move);
			std::rotate(moveList.begin(), it, it + 1);
		}
	}

	for (const auto& m : moveList) {
		doMove(m, copy);
		mySearch.nodes++;
		short score = -negamaxAB<false>(copy, depth - 1, -beta, -alpha, subPV);
		undoMove(m, copy, p);

		if (score >= beta) {
			bestScore = score;
			bestMove = m;
			break;
		}	
		
		if (score > bestScore) { // Beware: unlike negamaxRoot(), here it seems better only 'greather than'
			bestScore = score;
			bestMove = m;
			pv[0] = bestMove;
			memcpy(pv + 1, subPV, 63 * sizeof(Move));
			pv[63] = 0;

			if (score > alpha)
				alpha = score;
		}
	}

	/* we only update TT if there is a best move. In other words, if it's mate and therefore
	there is not any best move (or, hence, any move at all), we do not want to update
	TT, because otherwise we would have a dummy entry with score MATE or -MATE and move = 0, polluting TT! */
	if (bestMove) {
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
	}

	mySearch.bestMove = bestMove; 
	return bestScore;
}


const short quiescence(Position const& p, short alpha, short beta)
{
	short stand_pat = lazyEval(p);
	
	if (stand_pat >= beta)
		return beta;

	if (alpha < stand_pat)
		alpha = stand_pat;

	Position copy = p;
	
	std::vector<Move> moves = moveGenQS(copy), moveList{};
	moves = pruneIllegal(moves, copy);
	moveList = ordering(moves);

	for (const auto& m : moveList) {
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