#ifndef DIVIDE_H
#define DIVIDE_H

#include "defs.h"
#include "movegen.h"
#include "Position.h"
#include <iostream>
#include <cstdint>

template<bool Root>
uint64_t divide(short depth, Position &p)
{
	std::vector<Move> moveList = moveGeneration(p);
	moveList.reserve(MAX_PLY);
	extern std::map<Square, std::string> squareToStringMap;
	uint64_t cnt, nodes = 0;
	Position copy = p;
	p.storeState(depth);
	// pruneIllegal(moveList, p);
	const bool leaf = (depth == 1);

	for (const auto& m : moveList)
	{	
		if (Root && depth == 0)
			cnt = 1, nodes++;

		else {
			Color c = Color(((C64(1) << 1) - 1) & (m >> 12));
			doMove(m, p);

			if (underCheck(c, p) == 0) { // if move is legal...
				cnt = leaf ? 1 : divide<false>(depth - 1, p);
				nodes += cnt;
			}

			// undoMove(m, copy, p);
			undoMove(m, p);
			p.restoreState(depth);
		}

		if (Root) {
			Square squareFrom{}, squareTo{};
			squareFrom = Square(((C64(1) << 6) - 1) & (m >> 19));
			squareTo = Square(((C64(1) << 6) - 1) & (m >> 13));
			ushort promotedTo = ((C64(1) << 3) - 1) & (m);

			if (promotedTo)
				switch (promotedTo) {
				case PAWN_TO_QUEEN:
					std::cout << "q";
					break;
				case PAWN_TO_KNIGHT:
					std::cout << "n";
					break;
				case PAWN_TO_ROOK:
					std::cout << "r";
					break;
				case PAWN_TO_BISHOP:
					std::cout << "b";
					break;
				}

			std::cout << squareToStringMap[squareFrom] << squareToStringMap[squareTo];
			std::cout << ": " << cnt << std::endl;
		}
	}
	return nodes;
}

#endif
