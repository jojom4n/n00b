#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "defs.h"
#include "enums.h"

class Position;

namespace Zobrist {
	void init();
	uint64_t fill(Position const &p);
	uint64_t getKey(Color const& c, Piece const& p, Square const& sq);
	uint64_t getKey(Square const& sq);
	uint64_t getKey(Color const& c);
	uint64_t getKey(Color const& c, Castle const& castle);
	uint64_t getKey(ushort depth);
};

#endif
