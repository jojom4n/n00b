#include "pch.h"
#include "protos.h"
#include <iostream>

unsigned long long perft(short depth, Position const &p)
{
	unsigned long long nodes = 0;
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;

	if ((depth == 0) || (p.getCheckmate()))
		return 1;

	for (auto &elem : moveList)
	{
		doMove(elem, copy);
		nodes += perft(depth - 1, copy);
		undoMove(elem, copy, p);
	}
	
	return nodes;
}


/* void divide(short depth, Position const & p)
{
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;
	for (auto &elem : moveList) {
		Square squareFrom{}, squareTo{};
		squareFrom = Square(((C64(1) << 6) - 1) & (elem >> 20));
		squareTo = Square(((C64(1) << 6) - 1) & (elem >> 14));
		ushort promotedTo = ((C64(1) << 3) - 1) & (elem >> 1);
		std::cout << squareToStringMap[squareFrom] << squareToStringMap[squareTo];
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
		perft(depth, p);
		totalNodes += nodes;
		std::cout << ": " << nodes << std::endl;
		// undoMove(elem, copy, p);
	}
	std::cout << ": " << nodes << std::endl;
} */