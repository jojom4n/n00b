#include "pch.h"
#include "moveorder.h"
#include "enums.h"
#include "evaluation.h"
#include "makemove.h"
#include "search.h"
#include <map>

extern const std::map<Piece, ushort> g_pieceValue; // see evaluation.cpp (and its header)

std::vector<Move> ordering(std::vector<Move> const& moves, Position const& p)
{
	std::vector<Move> moveList;
	moveList.reserve(MAX_PLY);

	moveList = mvv_lva(moves);
	
	for (auto elem : moves) {
		ushort moveType = ((C64(1) << 3) - 1) & (elem >> 6); //is move a capture?
		if (!(moveType == CAPTURE))
			moveList.push_back(elem);
	}

	return moveList;
}


std::vector<Move> mvv_lva(std::vector<Move> const& moves) 
{
	std::vector<Move> captures;
	captures.reserve(MAX_PLY);
	std::multimap<short, Move, std::greater<int>> sortedMap{};

	for (auto const& elem : moves) {
		ushort moveType = ((C64(1) << 3) - 1) & (elem >> 6); //is move a capture?

		if (moveType == CAPTURE) {
			Piece attacker = static_cast<Piece>(((C64(1) << 3) - 1) & (elem >> 9));
			Piece victim = static_cast<Piece>(((C64(1) << 3) - 1) & (elem >> 3));
			short score = g_pieceValue.at(victim) - g_pieceValue.at(attacker);
			sortedMap.insert(std::pair<short, Move>(score, elem));
		}

	}

	for (std::multimap<short, Move>::iterator it = sortedMap.begin(); it != sortedMap.end(); ++it)
		captures.push_back((*it).second);

	return captures;
}