#include "pch.h"
#include "tt.h"

namespace TT
{
	std::array<TTEntry, TT_SIZE> table{};

	void Store(struct Search const& g_Search)
	{
		uint32_t key = static_cast<uint32_t>(g_Search.pos.getZobrist());
		unsigned long long index = key % TT_SIZE;

		if (table[index].key) { // a previous key exists at the index

			if (table[index].key == key && table[index].depth < g_Search.ply) {  // replace same entry, if at minor depth
				table[index].key = key;
				table[index].move = g_Search.bestMove;
				table[index].score = g_Search.bestScore;
				table[index].depth = static_cast<uint8_t>(g_Search.ply);
				table[index].age = static_cast<uint8_t>(g_Search.pos.getMoveNumber());
			}

			if (table[index].age < g_Search.pos.getMoveNumber()) {  // if the entry is old, replace it whatever in case of collision
				table[index].key = key;
				table[index].move = g_Search.bestMove;
				table[index].score = g_Search.bestScore;
				table[index].depth = static_cast<uint8_t>(g_Search.ply);
				table[index].age = static_cast<uint8_t>(g_Search.pos.getMoveNumber());
			}

		}

		else {  // a previous key does not exist at index position of the table
			table[index].key = key;
			table[index].move = g_Search.bestMove;
			table[index].score = g_Search.bestScore;
			table[index].depth = static_cast<uint8_t>(g_Search.ply);
			table[index].age = static_cast<uint8_t>(g_Search.pos.getMoveNumber());
		}
	}


	void Store(TTEntry const& entry)
	{
		uint32_t key = static_cast<uint32_t>(entry.key);
		unsigned long long index = key % TT_SIZE;

		if (table[index].key) { // a previous key exists at the index

			if (table[index].key == key && table[index].depth < entry.depth) {  // replace same entry, if at minor depth
				table[index].key = key;
				table[index].move = entry.move;
				table[index].score = entry.score;
				table[index].depth = static_cast<uint8_t>(entry.depth);
				table[index].age = static_cast<uint8_t>(entry.age);
			}

			if (table[index].age < entry.age) {  // if the entry is old, replace it whatever in case of collision
				table[index].key = key;
				table[index].move = entry.move;
				table[index].score = entry.score;
				table[index].depth = static_cast<uint8_t>(entry.depth);
				table[index].age = static_cast<uint8_t>(entry.age);
			}

		}

		else {  // a previous key does not exist at index position of the table
			table[index].key = key;
			table[index].move = entry.move;
			table[index].score = entry.score;
			table[index].depth = static_cast<uint8_t>(entry.depth);
			table[index].age = static_cast<uint8_t>(entry.age);
		}
	}

}