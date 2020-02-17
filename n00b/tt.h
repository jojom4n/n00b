#ifndef TT_H
#define TT_H

#include "params.h"
#include "search.h"

struct TTEntry {
	uint32_t key{}; // zobrist key - reduced to 32-bit
	uint8_t depth{}; // depth - 8-bit
	Move move{}; // best move - 32-bit
	int16_t score{}; // score for move - 16-bit
	unsigned char nodeType{}; // type of node (exact, fail-high, fail-low)
	uint8_t age{}; // age (6-bit)
};

namespace TT {
	extern std::array<TTEntry, TT_SIZE> table;
	void Store(TTEntry const& entry);
}

#endif
