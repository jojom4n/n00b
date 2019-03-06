#include "pch.h"
#include <iostream>
#include "protos.h"
#include "Position.h"

const Move calculateBestMove(Position const& p, short depth)
{
	Position copy = p;
	Color turn = copy.getTurn();
	std::vector<Move> moveList = moveGeneration(copy);
	Move bestMove{};
	short bestValue = -9999;

	for (auto& m : moveList) {
		short boardValue;
		doMove(m, copy);
		(turn == WHITE) ? boardValue = evaluate(copy) : boardValue = -evaluate(copy);
		undoMove(m, copy, p);

		if (boardValue > bestValue && underCheck(turn, copy) == 0) {
			bestValue = boardValue;
			bestMove = m;
		}
	}

	return bestMove;
}