#ifndef SEARCH_H
#define SEARCH_H

#include "defs.h"
#include "params.h"
#include "Position.h"
#include <list>
#include <vector>

#define MATE 32000 + MAX_PLY
#define MATE_IN_MAX (MATE - MAX_PLY)
#define ALPHA -MATE
#define BETA MATE

#define RFP_DEPTH 8 // for reverse futility pruning
#define RFP_MARGIN 75 // margin = 3/4 a pawn

#define ALPHA_PRUNING_DEPTH 5 // for alpha pruning
#define ALPHA_PRUNING_MARGIN 300 // margin = a piece 

#define MARGIN 50 // for futility pruning - MARGIN = half a pawn
#define EXTENDED_MARGIN 500 // for extended futility pruning - EXTENDED_MARGIN = a rook

// for extended null-move pruning
#define MAX_R 4
#define MIN_R 3
#define NMP_DEPTH 2

struct Search {
	Position pos;
	short bestScore, depth, height;
	unsigned long long nodes, ttHits, ttUseful;
	Move bestMove{}, pv[MAX_PLY];
	std::array<std::array<Move, 3>, MAX_PLY> killerMoves;
};

const Move iterativeSearch(Position& p, short const& depth);

/* template<bool nullMove>
const short pvs(Position& p, short depth, short alpha, short beta, Move* pv); */

template<bool nullMove>
const short newPVS(Position& p, short const& depth, short alpha, short const& beta, Move* pv);

const short quiescence(Position p, short alpha, short beta);
void initKillerMoves();

#endif
