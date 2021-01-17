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
	std::vector<Move> orderedList = shallowSort(moves, p);

	return orderedList;

	/* std::vector<Move> moveList{}, moveTemp{};
	
	moveTemp = mvv_lva(moves);
	moveList.insert(moveList.begin(), moveTemp.begin(), moveTemp.end());

	// add all the other generated moves but not ordered so far
	for (auto const& elem : moves) {
		if (std::find(moveList.begin(), moveList.end(), elem) == moveList.end())
			moveList.push_back(elem);
	}

	return moveList; */
}


std::vector<Move> shallowSort(std::vector<Move> const& moves, Position const& p)
{
	Position copy = p;

	std::vector<Move> orderedList{};
	std::multimap<short, Move, std::less<int>> sortedMap{};

	for (auto const& elem : moves) {
		doMove(elem, copy);
		sortedMap.insert(std::pair<short, Move>(quiescence(copy, ALPHA, BETA), elem));
		undoMove(elem, copy, p);
	}

	for (std::multimap<short, Move>::iterator it = sortedMap.begin(); it != sortedMap.end(); ++it)
		orderedList.push_back((*it).second);

	return orderedList;
}


std::vector<Move> mvv_lva(std::vector<Move> const& moves) 
{
	std::vector<Move> captures{};
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