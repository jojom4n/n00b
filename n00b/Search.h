#ifndef SEARCH_H
#define SEARCH_H

#include "defs.h"
#include "Position.h"
#include <list>
#include <vector>

#define SHRT_INFINITY std::numeric_limits<short>::max()
#define MATE (std::numeric_limits<short>::max() / 2)
#define ALPHA -SHRT_INFINITY
#define BETA SHRT_INFINITY
#define R 3 // for null-move pruning

struct Search {
	Position pos;
	short bestScore;
	unsigned long long nodes, ttHits, ttUseful;
	Move bestMove, pv[64];
};

const Move iterativeSearch(Position& p, short const& depth);
// Move negamaxRoot(short const& depth, Move* pv);

template<bool nullMove>
const short negamaxAB(Position const& p, short const& depth, short alpha, short beta, Move* pv);

const short quiescence(Position const& p, short alpha, short beta);


#endif
