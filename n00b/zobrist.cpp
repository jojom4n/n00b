#include "pch.h"
#include "defs.h"
#include "protos.h"
#include <random>

using namespace Zobrist;

constexpr uint64_t maxU64 = 0xFFFFFFFFFFFFFFFF;
uint64_t zobristKeys[2][6][64];
uint64_t zobristCastle[2][4];
uint64_t zobristEnPassant[64];
uint64_t zobristBlackToMove;

void Zobrist::init()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint64_t> randomNumbers(0, maxU64);

	for (ushort i = 0; i < 6; i++)
		for (ushort j = 0; j < 64; j++) {
			zobristKeys[WHITE][i][j] = randomNumbers(gen);
			zobristKeys[BLACK][i][j] = randomNumbers(gen);
		}

	for (ushort i = 0; i < 4; i++)
		zobristCastle[WHITE][i] = randomNumbers(gen);

	for (ushort i = 0; i < 64; i++)
		zobristEnPassant[i] = randomNumbers(gen);

	zobristBlackToMove = randomNumbers(gen);
}


uint64_t Zobrist::fill(Position const &p) {
	uint64_t result{};

	for (Color c = BLACK; c <= WHITE; c++) {

		for (Piece piece = KING; piece <= PAWN; piece++) {
			Bitboard bb = p.getPieces(c, piece);

			while (bb) {
				Square sq = Square(bitscan_reset(bb));
				result ^= zobristKeys[c][piece][sq];
			} // end while

		} // end nested for

		result ^= zobristCastle[WHITE][p.getCastle(WHITE)];
		result ^= zobristCastle[BLACK][p.getCastle(BLACK)];

		result ^= zobristEnPassant[p.getEnPassant()];

		if (p.getTurn() == BLACK)
			result ^= zobristBlackToMove;

	}  // end main for

	return result;
}  

uint64_t Zobrist::getKey(Color const &c, Piece const &p, Square const &sq)
{
	return zobristKeys[c][p][sq];
}