#include "pch.h"
#include <iostream>
#include "protos.h"
#include "defs.h"
#include "Position.h"

constexpr int PERFT_CACHE_SIZE = 0xF4240;


unsigned long long perft(short depth, Position& p)
{
	unsigned long long nodes{};
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;
	moveList = pruneIllegal(moveList, copy);

	static std::array<perftCache, PERFT_CACHE_SIZE> cache;

	if (depth <= 0 || moveList.size() == 0)
		return 0;

	for (auto& elem : moveList) {
		unsigned long long partialNodes;
		Square squareFrom = Square(((C64(1) << 6) - 1) & (elem >> 19));
		Square squareTo = Square(((C64(1) << 6) - 1) & (elem >> 13));
		doMove(elem, copy);
		partialNodes = perft(depth - 1, copy, cache);
		nodes += partialNodes;
		std::cout << squareToStringMap[squareFrom] << squareToStringMap[squareTo];
		std::cout << ": " << partialNodes << std::endl;
		undoMove(elem, copy, p);
	}

	return nodes;
}

template<size_t PERFT_CACHE_SIZE>
static unsigned long long perft(short depth, Position& p, std::array<perftCache, PERFT_CACHE_SIZE>& cache)
{
	unsigned long long nodes{};
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;
	moveList = pruneIllegal(moveList, copy);

	if (depth == 0)
		return 1;

	if (depth > 1) {
		if (cache[( copy.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].key 
			== (copy.getZobrist() ^ Zobrist::getKey(depth)))
			nodes = cache[( copy.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].nodes;
			// std::cout << "Zobrist!\n" << std::endl;
		
		if (nodes > 0)
			return nodes;
	}

	if (depth == 1 || moveList.size() == 0)
		return moveList.size();

	for (auto& elem : moveList) {
		doMove(elem, copy);
		nodes += perft(depth - 1, copy, cache);
		undoMove(elem, copy, p);
	}

	if (depth > 1) {
			// cache[copy.getZobrist() % PERFT_CACHE_SIZE].depth = depth;
			cache[( copy.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].key = copy.getZobrist() ^ Zobrist::getKey(depth);
			cache[( copy.getZobrist() ^ Zobrist::getKey(depth) ) % PERFT_CACHE_SIZE].nodes = nodes;
		}

	return nodes;
}


/*








unsigned long long perft(short depth, Position &p)
{
	unsigned long long nodes{};
	std::vector<Move> moveList = moveGeneration(p);
	Position copy = p;
	moveList = pruneIllegal(moveList, copy);

	if (depth == 1) {
		// moveList = pruneIllegal(moveList, copy);
		return moveList.size();
	}

	for (auto &elem : moveList)
	{
		doMove(elem, copy);
		nodes += perft(depth - 1, copy);
		undoMove(elem, copy, p);
	}

	return nodes;
}


/* uint64_t cacheGen(Move const& m, Position const& p)
{
	Square squareFrom{}, squareTo{};
	ushort moveType = ((C64(1) << 3) - 1) & (m >> 6);
	squareFrom = Square(((C64(1) << 6) - 1) & (m >> 19));
	squareTo = Square(((C64(1) << 6) - 1) & (m >> 13));
	Color color = Color(((C64(1) << 1) - 1) & (m >> 12));
	Piece piece = Piece(((C64(1) << 3) - 1) & (m >> 9));
	Piece captured = Piece(((C64(1) << 3) - 1) & (m >> 3));
	ushort promotedTo = ((C64(1) << 3) - 1) & (m);

	uint64_t zobristMove = p.getZobrist();

	switch (moveType)
	{
	case QUIET:
		zobristMove ^= Zobrist::getKey(color, piece, squareFrom);
		zobristMove ^= Zobrist::getKey(color, piece, squareTo);

		if ((piece == PAWN) && ((squareFrom / 8) == RANK_2) && ((squareTo / 8) == RANK_4)
			|| (piece == PAWN) && ((squareFrom / 8) == RANK_7) && ((squareTo / 8) == RANK_5))
			(color == WHITE) ? zobristMove ^= Zobrist::getKey(Square(squareTo - 8)) : zobristMove ^= Zobrist::getKey(Square(squareTo + 8));

		if (p.getTurn() == WHITE)
			zobristMove ^= Zobrist::getKey(BLACK);

		break;
	case CAPTURE:
		zobristMove ^= Zobrist::getKey(color, piece, squareFrom);
		zobristMove ^= Zobrist::getKey(Color(!color), captured, squareTo);
		zobristMove ^= Zobrist::getKey(color, piece, squareTo);

		if (p.getTurn() == WHITE)
			zobristMove ^= Zobrist::getKey(BLACK);

		break;
	case PROMOTION:
		zobristMove ^= Zobrist::getKey(color, piece, squareFrom);

		if (!(captured == NO_PIECE)) 
			zobristMove ^= Zobrist::getKey(Color(!color), captured, squareTo);
		
		switch (promotedTo) {
		case PAWN_TO_KNIGHT:
			zobristMove ^= Zobrist::getKey(color, KNIGHT, squareTo);
			break;
		case PAWN_TO_QUEEN:
			zobristMove ^= Zobrist::getKey(color, QUEEN, squareTo);
			break;
		case PAWN_TO_BISHOP:
			zobristMove ^= Zobrist::getKey(color, BISHOP, squareTo);
			break;
		case PAWN_TO_ROOK:
			zobristMove ^= Zobrist::getKey(color, ROOK, squareTo);
			break;
		}

		if (p.getTurn() == WHITE)
			zobristMove ^= Zobrist::getKey(BLACK);

		break;
	case CASTLE_Q: {
		zobristMove ^= Zobrist::getKey(color, piece, squareFrom);

		(color == WHITE) ? zobristMove ^= Zobrist::getKey(color, ROOK, A1) : zobristMove ^= Zobrist::getKey(color, ROOK, A8);

		zobristMove ^= Zobrist::getKey(color, piece, squareTo);

		(color == WHITE) ? zobristMove ^= Zobrist::getKey(color, ROOK, D1) : zobristMove ^= Zobrist::getKey(color, ROOK, D8);

		zobristMove ^= Zobrist::getKey(color, NONE);

		if (p.getTurn() == WHITE)
			zobristMove ^= Zobrist::getKey(BLACK);

		break;	}
	case CASTLE_K: {
		zobristMove ^= Zobrist::getKey(color, piece, squareFrom);

		(color == WHITE) ? zobristMove ^= Zobrist::getKey(color, ROOK, H1) : zobristMove ^= Zobrist::getKey(color, ROOK, H8);

		zobristMove ^= Zobrist::getKey(color, piece, squareTo);

		(color == WHITE) ? zobristMove ^= Zobrist::getKey(color, ROOK, F1) : zobristMove ^= Zobrist::getKey(color, ROOK, F8);

		zobristMove ^= Zobrist::getKey(color, NONE);

		if (p.getTurn() == WHITE)
			zobristMove ^= Zobrist::getKey(BLACK);

		break; }
	case EN_PASSANT:
		zobristMove ^= Zobrist::getKey(color, piece, squareFrom);

		(color == WHITE) ? zobristMove ^= Zobrist::getKey(Color(!color), PAWN, Square(squareTo - 8)) 
			: zobristMove ^= Zobrist::getKey(Color(!color), PAWN, Square(squareTo + 8));
		
		zobristMove ^= Zobrist::getKey(color, piece, squareTo);

		zobristMove ^= Zobrist::getKey(SQ_EMPTY);

		if (p.getTurn() == WHITE)
			zobristMove ^= Zobrist::getKey(BLACK);

		break;
	}

	if (!(moveType == EN_PASSANT) && !(p.getEnPassant() == SQ_EMPTY)) {
		Color temp;
		(p.getEnPassant() / 8 == RANK_3) ? temp = WHITE : temp = BLACK;

		if (!(temp == color))
			zobristMove ^= Zobrist::getKey(SQ_EMPTY);
	}

	// if King moves, no more castle
	if (!(moveType == CASTLE_Q) && !(moveType == CASTLE_K) && (piece == KING))
		zobristMove ^= Zobrist::getKey(color, NONE);

	// if Rook moves, no more castle on that side
	switch (color)
	{
	case WHITE:
		if (!(moveType == CASTLE_K) && piece == ROOK && squareFrom == H1
			&& (p.getCastle(WHITE) == KINGSIDE || p.getCastle(WHITE) == ALL))
			zobristMove ^= Zobrist::getKey(WHITE, Castle(p.getCastle(WHITE) - KINGSIDE));

		if (!(moveType == CASTLE_Q) && piece == ROOK && squareFrom == A1
			&& (p.getCastle(WHITE) == QUEENSIDE || p.getCastle(WHITE) == ALL))
			zobristMove ^= Zobrist::getKey(WHITE, Castle(p.getCastle(WHITE) - QUEENSIDE));

		break;
	case BLACK:
		if (!(moveType == CASTLE_K) && piece == ROOK && squareFrom == H8
			&& (p.getCastle(BLACK) == KINGSIDE || p.getCastle(BLACK) == ALL))
			zobristMove ^= Zobrist::getKey(BLACK, Castle(p.getCastle(BLACK) - KINGSIDE));

		if (!(moveType == CASTLE_Q) && piece == ROOK && squareFrom == A8
			&& (p.getCastle(BLACK) == QUEENSIDE || p.getCastle(BLACK) == ALL))
			zobristMove ^= Zobrist::getKey(BLACK, Castle(p.getCastle(BLACK) - QUEENSIDE));

		break;
	}

	// if Rook gets captured, no more castle, if previously possible
	if (color == BLACK && captured == ROOK && squareTo == H1
		&& (p.getCastle(WHITE) == KINGSIDE || p.getCastle(WHITE) == ALL))
		zobristMove ^= Zobrist::getKey(WHITE, Castle(p.getCastle(WHITE) - KINGSIDE));

	else if (color == BLACK && captured == ROOK && squareTo == A1
		&& (p.getCastle(WHITE) == QUEENSIDE || p.getCastle(WHITE) == ALL))
		zobristMove ^= Zobrist::getKey(WHITE, Castle(p.getCastle(WHITE) - QUEENSIDE));

	else if (color == WHITE && captured == ROOK && squareTo == H8
		&& (p.getCastle(BLACK) == KINGSIDE || p.getCastle(BLACK) == ALL))
		zobristMove ^= Zobrist::getKey(BLACK, Castle(p.getCastle(BLACK) - KINGSIDE));

	else if (color == WHITE && captured == ROOK && squareTo == A8
		&& (p.getCastle(BLACK) == QUEENSIDE || p.getCastle(BLACK) == ALL))
		zobristMove ^= Zobrist::getKey(BLACK, Castle(p.getCastle(BLACK) - QUEENSIDE));

	return zobristMove;
} */
