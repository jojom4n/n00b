#include "pch.h"
#include "zobrist.h"
#include "bitscan.h"
#include "overloading.h"
#include "Position.h"
#include "params.h"
#include <random>

constexpr uint64_t maxU64 = 0xFFFFFFFFFFFFFFFF;
uint64_t zobristKeys[2][6][64];
uint64_t zobristCastle[2][4];
uint64_t zobristEnPassant[8];
uint64_t zobristSideToMove[2];
uint64_t zobristDepth[MAX_PLY];


namespace Zobrist {
	void init()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint64_t> randomNumbers(0, maxU64);

		for (Piece i : { KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN })
			for (ushort j = 0; j < SQ_NUMBER; j++) {
				zobristKeys[WHITE][i][j] = randomNumbers(gen);
				zobristKeys[BLACK][i][j] = randomNumbers(gen);
			}

		for (Castle i : { NONE, KINGSIDE, QUEENSIDE, ALL }) {
			zobristCastle[WHITE][i] = randomNumbers(gen);
			zobristCastle[BLACK][i] = randomNumbers(gen);
		}

		for (ushort i = 0; i < RANK_NUMBER; i++)
			zobristEnPassant[i] = randomNumbers(gen);

		for (Color i : {BLACK, WHITE})
			zobristSideToMove[i] = randomNumbers(gen);

		for (ushort i = 0; i < MAX_PLY; i++) // TODO - Why 10?
			zobristDepth[i] = randomNumbers(gen);
	}


	uint64_t fill(Position const &p) {
		uint64_t result{};

		for (Color c : {BLACK, WHITE}) {

			for (Piece piece : {KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN }) {
				Bitboard bb = p.getPieces(c, piece);

				while (bb) {
					Square sq = Square(bitscan_reset(bb));
					result ^= zobristKeys[c][piece][sq];
				} 

			} 

			result ^= zobristCastle[WHITE][static_cast<ushort>(p.getCastle(WHITE))];
			result ^= zobristCastle[BLACK][static_cast<ushort>(p.getCastle(BLACK))];

			if (p.getEnPassant() != SQ_EMPTY) 
				result ^= zobristEnPassant[p.getEnPassant() % 8];

			result ^= zobristSideToMove[p.getTurn()];

		}  // end main for

		return result;
	}

	
	uint64_t getKey(Color const& c, Piece const& p, Square const& sq) // for put/remove pieces
	{
		return zobristKeys[c][p][sq];
	}


	uint64_t getKey(Square const& sq) // for en passant
	{
		return zobristEnPassant[sq];
	}


	uint64_t getKey(Color const& c) // for side to move
	{
		return zobristSideToMove[c];
	}


	uint64_t getKey(Color const& c, Castle const& castle) // for castle
	{
		return zobristCastle[c][static_cast<ushort>(castle)];
	}

	uint64_t getKey(ushort depth)
	{
		return zobristDepth[depth];
	}

}