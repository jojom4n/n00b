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
	std::list<std::string> pv;
	unsigned long long nodes, ttHits, ttUseful;
	Move bestMove;
	bool flagMate;
};

const Move iterativeSearch(Position& p, short const& depth);
Move negamaxRoot(struct Search& mySearch, short const& depth);

template<bool nullMove>
const short negamaxAB(Position const& p, short const& depth, short alpha, short beta, unsigned long long& nodes, std::list<std::string>& childPv);

const short quiescence(Position const& p, short alpha, short beta, unsigned long long& nodes);


#endif
