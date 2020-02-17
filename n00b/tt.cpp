#include "pch.h"
#include "tt.h"

namespace TT
{
	std::array<TTEntry, TT_SIZE> table;
	
	void Store(TTEntry const& entry)
	{
		unsigned long long index = entry.key % TT_SIZE;

		if (table[index].key) { // a previous key exists at the index

			if (table[index].key == entry.key && table[index].depth < entry.depth) {  // replace same entry, if at minor depth
				table[index].key = entry.key;
				table[index].move = entry.move;
				table[index].score = entry.score;
				table[index].nodeType = entry.nodeType;
				table[index].depth = static_cast<uint8_t>(entry.depth);
				table[index].age = static_cast<uint8_t>(entry.age);
			}

			if (table[index].age < entry.age) {  // if the entry is old, replace it whatever in case of collision
				table[index].key = entry.key;
				table[index].move = entry.move;
				table[index].score = entry.score;
				table[index].nodeType = entry.nodeType;
				table[index].depth = static_cast<uint8_t>(entry.depth);
				table[index].age = static_cast<uint8_t>(entry.age);
			}

		}

		else {  // a previous key does not exist at index position of the table
			table[index].key = entry.key;
			table[index].move = entry.move;
			table[index].score = entry.score;
			table[index].nodeType = entry.nodeType;
			table[index].depth = static_cast<uint8_t>(entry.depth);
			table[index].age = static_cast<uint8_t>(entry.age);
		}
	}

}