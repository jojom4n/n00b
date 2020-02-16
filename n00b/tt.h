#ifndef TT_H
#define TT_H

#include "search.h"

#define TT_BIT (8000000 / 96) // MB (8 mln bits) / 96-bit TT

struct TTEntry {
	uint32_t key{}; // zobrist key - reduced to 32-bit
	uint8_t depth{}; // depth - 8-bit
	Move move{}; // best move - 32-bit
	int16_t score{}; // score for move - 16-bit
	unsigned char nodeType{}; // type of node (exact, fail-high, fail-low)
	uint8_t age{}; // age (6-bit)
};

namespace TT {
	extern std::vector<TTEntry> table;
	void Store(struct Search const& Search);
	void Store(TTEntry const& entry);
}

#endif
