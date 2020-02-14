#include "pch.h"
#include "protos.h"
#include "params.h"
#include "Position.h"

namespace TT {
	extern std::vector<TTEntry> table(TT_SIZE);

	void update(unsigned long long const& index, uint32_t const& hashKey, Move const& m, short const& score, ushort const& depth, ushort const& age)
	{
		if (table[index].key) { // a previous key exists at the index
			
			if (table[index].key == hashKey && table[index].depth < depth) {  // replace same entry, if at minor depth
				table[index].key = hashKey;
				table[index].move = m;
				table[index].score = score;
				table[index].depth = depth;
				table[index].age = age;
			}

			if (table[index].age < age) {  // if the entry is old, replace it in case of collision
				table[index].key = hashKey;
				table[index].move = m;
				table[index].score = score;
				table[index].depth = depth;
				table[index].age = age;
			}

		}

		else {
			table[index].key = hashKey;
			table[index].move = m;
			table[index].score = score;
			table[index].depth = depth;
			table[index].age = age;
		}
	}
}
