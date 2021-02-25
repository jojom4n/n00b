#ifndef SEARCH_H
#define SEARCH_H

#include "defs.h"
#include "params.h"
#include "Position.h"
#include <list>
#include <vector>

#define SHRT_INFINITY std::numeric_limits<short>::max()
#define MATE (std::numeric_limits<short>::max() / 2)
#define ALPHA -SHRT_INFINITY
#define BETA SHRT_INFINITY
#define MARGIN 50 // for futility pruning - MARGIN = half a pawn
#define EXTENDED_MARGIN 500 // for extended futility pruning - EXTENDED_MARGIN = a rook

struct Search {
	Position pos;
	short bestScore, depth;
	unsigned long long nodes, ttHits, ttUseful;
	Move bestMove{}, pv[MAX_PLY];
	std::array<std::array<Move, 3>, MAX_PLY> killerMoves;
};

const Move iterativeSearch(Position& p, short const& depth);

template<bool nullMove>
const short pvs(Position& p, short const& depth, short alpha, short beta, Move* pv);

const short quiescence(Position p, short alpha, short beta);
const ushort determineR(short const &depth, Position const &p);

#endif
