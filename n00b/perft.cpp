#include "pch.h"
#include "protos.h"


unsigned long long perft(short depth, Position &p)
{
	unsigned long long nodes = 0;
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;

	if (depth == 1) {
		moveList = pruneIllegal(moveList, copy);
		return moveList.size();
	}

	for (auto &elem : moveList)
	{
		doMove(elem, copy);
		if (!underCheck(Color(!copy.getTurn()), copy))
			nodes += perft(depth - 1, copy);
		undoMove(elem, copy, p);
	}

	return nodes;
}