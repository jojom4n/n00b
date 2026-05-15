#ifndef TT_H
#define TT_H

#include "params.h"
#include "search.h"

enum TTNodeType : unsigned char {
	EXACT = 0,
	LOWER_BOUND = 1,
	UPPER_BOUND = 2
};

struct TTEntry {
	uint32_t key{}; // zobrist key - reduced to 32-bit
	uint8_t depth{}; // depth - 8-bit
	Move move{}; // best move - 32-bit
	int16_t score{}; // score for move - 16-bit
	TTNodeType nodeType{}; // type of node (exact, fail-high, fail-low)
	uint8_t age{}; // age (6-bit)
};

namespace TT {
	extern std::vector<TTEntry> table;
	void Store(const TTEntry& entry);
	const TTEntry* Lookup(uint32_t key, uint8_t depth);
}

#endif
