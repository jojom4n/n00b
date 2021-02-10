#include "pch.h"
#include "perft.h"
#include "enums.h"
#include "makemove.h"
#include "movegen.h"
#include "params.h"
#include "Position.h"
#include <iostream>
#include <map>

unsigned long long debug_nodes{};

unsigned long long perft(ushort const &depth, Position &p)
{
	if (depth <= 0)
		return 0;
	
	static std::array<perftCache, PERFT_CACHE_SIZE> cache;
	unsigned long long nodes{};
	extern std::map<Square, std::string> squareToStringMap; // see display.cpp
	std::vector<Move> moveList;
	moveList.reserve(MAX_PLY);
	moveList = moveGeneration(p);
	
	if (moveList.size() == 0)
		return 0;

	debug_nodes += moveList.size();
	p.storeState(depth);

	for (const auto& elem : moveList) {
			unsigned long long partialNodes;
			Color c = Color(((C64(1) << 1) - 1) & (elem >> 12));
			doMove(elem, p);

			if (!underCheck(c, p)) { // if move is legal...
				partialNodes = perft(depth - 1, p, cache);
				nodes += partialNodes;
			}

			undoMove(elem, p);
			p.restoreState(depth);
	}

	return nodes;
}

template<size_t PERFT_CACHE_SIZE>
static unsigned long long perft(ushort const &depth, Position &p, std::array<perftCache, PERFT_CACHE_SIZE> &cache)
{
	if (depth == 0)
		return 1;
	
	unsigned long long nodes{};
	std::vector<Move> moveList;
	moveList.reserve(MAX_PLY);
	moveList = moveGeneration(p);
	debug_nodes += moveList.size();
	pruneIllegal(moveList, p);

#ifndef NO_PERFT_CACHE
	if (depth > 1) {
		if (cache[( p.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].key
			== (p.getZobrist() ^ Zobrist::getKey(depth)))
			nodes = cache[( p.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].nodes;

		if (nodes > 0)
			return nodes;
	}
#endif

	if (depth == 1 || moveList.size() == 0)
		return moveList.size(); 

	p.storeState(depth);

	for (const auto &elem : moveList) {
		Color c = Color(((C64(1) << 1) - 1) & (elem >> 12));
		doMove(elem, p);
		
		// if (!underCheck(c, p)) // if move is legal...
			nodes += perft(depth - 1, p, cache);

		undoMove(elem, p);
		p.restoreState(depth);
	}

#ifndef NO_PERFT_CACHE
	if (depth > 1) {
			cache[( p.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].key = p.getZobrist() ^ Zobrist::getKey(depth);
			cache[( p.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].nodes = nodes;
	}
#endif

	return nodes;
}
