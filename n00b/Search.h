#ifndef SEARCH_H
#define SEARCH_H

#include "defs.h"
#include "Position.h"
#include <vector>

#define SHRT_INFINITY std::numeric_limits<short>::max()
#define MATE (std::numeric_limits<short>::max() / 2)
#define ALPHA -SHRT_INFINITY
#define BETA SHRT_INFINITY

struct Search {
	Position pos;
	unsigned short ply;
	short bestScore;
	unsigned long nodes;
	std::vector<Move> pv;
	Move currBestMove;
	Move bestMove;
	bool flagMate;
};

const Move iterativeSearch(Position& p, ushort const& depth);
void negamaxRoot(struct Search& Search, ushort const& rootDepth);
const short negamaxAB(Position const& p, ushort const& depth, short alpha, short beta, unsigned long& nodes, std::vector<Move>& childPv);
const short quiescence(Position const& p, short alpha, short beta, unsigned long& nodes);


#endif
