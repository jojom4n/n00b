#include "pch.h"
#include "tt.h"
#include "makemove.h"
#include "movegen.h"
#include <cstring>

namespace TT
{
	TTEntry* table = nullptr;

	void Init()
	{
		if (table == nullptr) {
			table = new TTEntry[TT_SIZE];
			Clear();
		}
	}

	void Clear()
	{
		if (table != nullptr) {
			for (size_t i = 0; i < TT_SIZE; ++i) {
				table[i].key.store(0, std::memory_order_relaxed);
				table[i].depth = 0;
				table[i].move = Move{};
				table[i].score = 0;
				table[i].nodeType = EXACT;
				table[i].age = 0;
			}
		}
	}

	void Deallocate()
	{
		if (table != nullptr) {
			delete[] table;
			table = nullptr;
		}
	}

	void Store(uint32_t key, uint8_t depth, Move move, int16_t score, TTNodeType nodeType, uint8_t age)
	{
		if (table == nullptr) Init();

		unsigned long long index = key % TT_SIZE;
		TTEntry& slot = table[index];

		// Store all fields first (non-atomically)
		slot.depth = depth;
		slot.move = move;
		slot.score = score;
		slot.nodeType = nodeType;
		slot.age = age;

		// Atomically store the key last - this provides happens-before guarantee
		slot.key.store(key, std::memory_order_release);
	}

	const TTEntry* Lookup(uint32_t key, uint8_t depth)
	{
		if (table == nullptr) {
			return nullptr;
		}

		unsigned long index = key % TT_SIZE;
		TTEntry& slot = table[index];

		// Load the key atomically first to establish happens-before relationship
		uint32_t loaded_key = slot.key.load(std::memory_order_acquire);

		// If key doesn't match, return null
		if (loaded_key != key) {
			return nullptr;
		}

		// Perform double-check to prevent data tearing
		uint32_t double_check_key = slot.key.load(std::memory_order_acquire);
		
		// If the key changed during our check, return null
		if (double_check_key != key) {
			return nullptr;
		}

		// Verify that we got a valid entry
		if (slot.depth >= depth) {
			return &slot;
		}

		return nullptr;
	}
}

