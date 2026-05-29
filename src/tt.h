#ifndef TT_H
#define TT_H

#include "Position.h"
#include "defs.h"
#include <cstdint>
#include <atomic>

enum TTNodeType
{
	EXACT,
	LOWER,
	UPPER
};

struct TTEntry
{
	std::atomic<uint32_t> key;
	uint8_t depth;
	Move move;
	int16_t score;
	uint8_t nodeType;
	uint8_t age;
};

namespace TT
{
	void Init();
	void Store(uint32_t key, uint8_t depth, Move move, int16_t score, TTNodeType nodeType, uint8_t age);
	const TTEntry* Lookup(uint32_t key, uint8_t depth);
	void Clear();
	void Deinit();
}

#endif
