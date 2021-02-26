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

// for extended null-move pruning
#define MAX_R 4
#define MIN_R 3
#define DR 4

struct Search {
	Position pos;
	short bestScore, depth;
	unsigned long long nodes, ttHits, ttUseful;
	Move bestMove{}, pv[MAX_PLY];
	std::array<std::array<Move, 3>, MAX_PLY> killerMoves;
	std::array<std::array<std::array<ushort, SQ_NUMBER>, NO_PIECE>, ALL_COLOR> historyTbl;
};

const Move iterativeSearch(Position& p, short const& depth);

template<bool nullMove>
const short pvs(Position& p, short depth, short alpha, short beta, Move* pv);

const short quiescence(Position p, short alpha, short beta);
bool const isPV(short const& alpha, short const& beta);
void updateHistoryTBL(short const& depth, Move const& m, short const& beta, short const& score);
void updateKillerMoves(short const& depth, Move const& m);
short const determineLMR(short const& depth, ushort const& moveCount, Move const& m, Position const& p);

#endif
