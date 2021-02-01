#ifndef PERFT_H
#define PERFT_H

#include "defs.h"

class Position;

struct perftCache {
	unsigned long long key;
	unsigned long long nodes;
};

unsigned long long perft(ushort const& depth, Position& p, bool init = false);

template<size_t PERFT_CACHE_SIZE>
static unsigned long long perft(ushort const& depth, Position& p, std::array<perftCache, PERFT_CACHE_SIZE>& cache);

#endif
