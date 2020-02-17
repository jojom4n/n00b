#include "pch.h"
#include "moveorder.h"
#include "enums.h"
#include "evaluation.h"
#include <map>

extern const std::map<Piece, ushort> g_pieceValue; // see evaluation.cpp (and its header)

std::vector<Move> mvv_lva(std::vector<Move> const& origList) 
{
	std::vector<Move> moveCopy{};
	std::multimap<short, Move, std::greater<int>> sortedMap{};

	for (const auto& elem : origList) {
		ushort moveType = ((C64(1) << 3) - 1) & (elem >> 6); //is move a capture?
		
		if (moveType == CAPTURE) {
			Piece attacker = static_cast<Piece>(((C64(1) << 3) - 1) & (elem >> 9));
			Piece victim = static_cast<Piece>(((C64(1) << 3) - 1) & (elem >> 3));
			short score = g_pieceValue.at(victim) - g_pieceValue.at(attacker);
			sortedMap.insert(std::pair<short, Move>(score, elem));
		}
		
		for (std::multimap<short, Move>::iterator it = sortedMap.begin(); it != sortedMap.end(); ++it)
			std::vector<Move>::iterator it = std::find(moveCopy.begin(), moveCopy.end(), TTEntry.move);
		std::rotate(moveList.begin(), it, it + 1);
			
			moveCopy.push_back((*it).second);
		
		moveCopy.push_back(elem);
	}

	
	return moveCopy;
}