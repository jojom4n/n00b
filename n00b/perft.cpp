#include "pch.h"
#include <iostream>
#include "protos.h"
#include "defs.h"
#include "Position.h"

constexpr unsigned long long PERFT_CACHE_SIZE = 0x2DC6C0;

unsigned long long perft(short depth, Position& p, bool init)
{
	unsigned long long nodes{};
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;
	moveList = pruneIllegal(moveList, copy);

	static std::array<perftCache, PERFT_CACHE_SIZE> cache;

	if (depth <= 0 || moveList.size() == 0)
		return 0;

	for (const auto& elem : moveList) {
		unsigned long long partialNodes;
		Square squareFrom = Square(((C64(1) << 6) - 1) & (elem >> 19));
		Square squareTo = Square(((C64(1) << 6) - 1) & (elem >> 13));
		doMove(elem, copy);
		partialNodes = perft(depth - 1, copy, cache);
		nodes += partialNodes;
		
		if (!init) {
			std::cout << squareToStringMap[squareFrom] << squareToStringMap[squareTo];
			std::cout << ": " << partialNodes << std::endl;
		}

		undoMove(elem, copy, p);
	}

	return nodes;
}

template<size_t PERFT_CACHE_SIZE>
static unsigned long long perft(short depth, Position& p, std::array<perftCache, PERFT_CACHE_SIZE>& cache)
{
	unsigned long long nodes{};
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;
	moveList = pruneIllegal(moveList, copy);

	if (depth == 0)
		return 1;

	if (depth > 1) {
		if (cache[( copy.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].key 
			== (copy.getZobrist() ^ Zobrist::getKey(depth)))
			nodes = cache[( copy.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].nodes;
		
		if (nodes > 0)
			return nodes;
	}

	if (depth == 1 || moveList.size() == 0)
		return moveList.size();

	for (const auto& elem : moveList) {
		doMove(elem, copy);
		nodes += perft(depth - 1, copy, cache);
		undoMove(elem, copy, p);
	}

	if (depth > 1) {
			cache[( copy.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].key = copy.getZobrist() ^ Zobrist::getKey(depth);
			cache[( copy.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].nodes = nodes;
		}

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