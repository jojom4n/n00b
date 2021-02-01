#include "pch.h"
#include "perft.h"
#include "enums.h"
#include "makemove.h"
#include "movegen.h"
#include "params.h"
#include "Position.h"
#include <iostream>
#include <map>


unsigned long long perft(ushort depth, Position& p, bool init)
{
	unsigned long long nodes{};
	extern std::map<Square, std::string> squareToStringMap; // see display.cpp
	std::vector<Move> moveList;
	moveList.reserve(MOVES);
	moveList = moveGeneration(p);
	p.storeState();
	// Position copy = p;
	moveList = pruneIllegal(moveList, p);

	static std::array<perftCache, PERFT_CACHE_SIZE> cache;

	if (depth <= 0 || moveList.size() == 0)
		return 0;

	for (const auto& elem : moveList) {
		unsigned long long partialNodes;
		Square squareFrom = Square(((C64(1) << 6) - 1) & (elem >> 19));
		Square squareTo = Square(((C64(1) << 6) - 1) & (elem >> 13));
		doMove(elem, p);
		partialNodes = perft(depth - 1, p, cache);
		nodes += partialNodes;

		if (!init) {
			std::cout << squareToStringMap[squareFrom] << squareToStringMap[squareTo];
			std::cout << ": " << partialNodes << std::endl;
		}

		undoMove(elem, p);
	}

	return nodes;
}

template<size_t PERFT_CACHE_SIZE>
static unsigned long long perft(ushort depth, Position& p, std::array<perftCache, PERFT_CACHE_SIZE>& cache)
{
	unsigned long long nodes{};
	std::vector<Move> moveList;
	moveList.reserve(MOVES);
	moveList = moveGeneration(p);
	p.storeState();
	// Position copy = p;
	moveList = pruneIllegal(moveList, p);

	if (depth == 0)
		return 1;

	/* if (depth > 1) {
		if (cache[( p.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].key
			== (p.getZobrist() ^ Zobrist::getKey(depth)))
			nodes = cache[( p.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].nodes;

		if (nodes > 0)
			return nodes;
	} */

	if (depth == 1 || moveList.size() == 0)
		return moveList.size();

	for (const auto& elem : moveList) {
		doMove(elem, p);
		nodes += perft(depth - 1, p, cache);
		undoMove(elem, p);
	}

	/* if (depth > 1) {
			cache[( p.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].key = p.getZobrist() ^ Zobrist::getKey(depth);
			cache[( p.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].nodes = nodes;
		} */

	return nodes;
}


void perftInit()
{
	unsigned long long tmpCache{};
	Position perftCache;
	perftCache.setNew();
	tmpCache = perft(1, perftCache, true);
	tmpCache = perft(2, perftCache, true);
	tmpCache = perft(3, perftCache, true);
	tmpCache = perft(4, perftCache, true);
	tmpCache = perft(5, perftCache, true);
}