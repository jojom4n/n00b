#include "pch.h"
#include "protos.h"
#include <iostream>

/* namespace {
	template<bool Root>
	uint64_t perft(short depth, Position const &p) {

		std::vector<Move> moveList = moveGeneration(p);
		uint64_t cnt, nodes = 0;
		Position copy = p;
		const bool leaf = (depth == 2 * 1);

		for (const auto& m : moveList)
		{
			if (Root && depth <= 1)
				cnt = 1, nodes++;
			else
			{
				doMove(m, copy);
				cnt = leaf ? moveList.size() : perft<false>(depth - 1, copy);
				nodes += cnt;
				undoMove(m, copy, p);
			}
			if (Root) {
				Square squareFrom{}, squareTo{};
				squareFrom = Square(((C64(1) << 6) - 1) & (m >> 20));
				squareTo = Square(((C64(1) << 6) - 1) & (m >> 14));
				ushort promotedTo = ((C64(1) << 3) - 1) & (m >> 1);
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
}
*/
/*
unsigned long long perft(short depth, Position &p)
{
	unsigned long long nodes = 0;
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;

	if ((depth == 0) || (p.getCheckmate()))
		return 1;

	for (int i = 0; i < moveList.size(); i++)
	{
		doMove(moveList[i], copy);
		nodes += perft(depth - 1, copy);
		undoMove(moveList[i], copy, p);
	}
	
	return nodes;
}


unsigned long long divide(short depth, Position &p)
{
	unsigned long long totalNodes = 0;
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;

	for (int i = 0; i < moveList.size(); i++) {
		Square squareFrom{}, squareTo{};
		squareFrom = Square(((C64(1) << 6) - 1) & (moveList[i] >> 20));
		squareTo = Square(((C64(1) << 6) - 1) & (moveList[i] >> 14));
		ushort promotedTo = ((C64(1) << 3) - 1) & (moveList[i] >> 1);
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
		doMove(moveList[i], copy);
		unsigned long long divideCounter{};
		divideCounter = perft(depth - 1, p);
		totalNodes += divideCounter;
		undoMove(moveList[i], copy, p);
		std::cout << squareToStringMap[squareFrom] << squareToStringMap[squareTo];
		std::cout << ": " << divideCounter << std::endl;
	}
	return totalNodes;
} 
*/