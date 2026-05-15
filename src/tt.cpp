#include "pch.h"
#include "tt.h"
#include "makemove.h"
#include "movegen.h"

namespace TT
{
	std::vector<TTEntry> table;

	void Store(const TTEntry& entry)
	{
		if (table.empty()) {
			table.resize(TT_SIZE);
		}

		unsigned long long index = entry.key % TT_SIZE;
		TTEntry& slot = table[index];

		// Replace if:
		// 1. The slot is empty (key == 0)
		// 2. The new entry has a greater or equal depth
		// 3. The new entry has a greater age (to prevent stale entries)
		if (slot.key == 0 || entry.depth >= slot.depth || entry.age > slot.age) {
			slot = entry;
		}
	}

	const TTEntry* Lookup(uint32_t key, uint8_t depth)
	{
		if (table.empty()) return nullptr;

		unsigned long	index = key % TT_SIZE;
		const TTEntry& slot = table[index];

		if (slot.key == key && slot.depth >= depth) {
			return &slot;
		}

		return nullptr;
	}
}
