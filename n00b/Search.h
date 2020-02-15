#ifndef SEARCH_H
#define SEARCH_H

#include "Position.h"

struct g_Search {
	Position pos{};
	unsigned short ply{};
	short bestScore{};
	unsigned long nodes{};
	std::vector<Move> pv{};
	Move currBestMove{};
	Move bestMove;
	bool flagMate{};
};


#endif