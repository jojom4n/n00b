#include "pch.h"
#include "protos.h"

std::array<uint64_t, PERFT_CACHE_SIZE> perftCache({});

unsigned long long perft(short depth, Position &p)
{
	unsigned long long nodes = 0;
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;
	perftCache[p.getZobrist() % PERFT_CACHE_SIZE] = p.getZobrist();

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