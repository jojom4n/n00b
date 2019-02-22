#include "pch.h"
#include <cmath>
#include "Position.h"
#include "protos.h"

short negamax(Position &p, short depth) {
	short score{}, bestScore = -INFINITY;

	if (depth == 0)
		return evaluate(p);

	std::vector<Move> moveList = moveGeneration(p);

	for (int i = 0; i < moveList.size(); i++) {
		Move m = moveList[i];
		doMove(m, p);
		score = -negamax(p, depth - 1);
		undoMove(m, p);
		if (score > bestScore) bestScore = score;
	}
	
	return bestScore;
}