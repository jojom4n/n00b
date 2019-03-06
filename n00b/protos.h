#ifndef PROTOS_H
#define PROTOS_H

#include "defs.h"
#include <vector>

class Position;

// attack.cpp
void initAttacks();
void SlidingMaskEx();
void linesAttacks();
void raysAttacks();
void raysEx();
void kingMask();
void knightMask();


// magic.cpp
const Bitboard gen_blockerboard(ushort const &index, ushort const &bits, Bitboard b);
const Bitboard gen_r_attks(Square const &square, Bitboard const &blockerboard);
const Bitboard gen_b_attks(Square const &square, Bitboard const &blockerboard);
void rookMagic();
void bishopMagic();


// game.cpp
void newGame();
void readCommand(std::stringstream &inputStream, Position &board);


// fen.cpp
const bool fenValidate(std::stringstream &fen);
void fenParser(std::stringstream &fen, Position &board);


//display.cpp
void displayBoard(Position const &board);
void displayMoveList(Position const &board, std::vector<Move> const &m);
const char printPiece(PieceID const &ID);


// movegen.cpp
const std::vector<Move> moveGeneration(Position &p);
const std::vector<Move> generateOnlyKing(Color const &c, Position const &p);
const Bitboard pawnMoves(Position const &p, Square const &from);
void castleMoves(Position const &p);
void enPassant(Position &p, Square const &enPassant, Color const &c);
const MoveType setType(Piece const &piece, Bitboard const &occ, Color const & c, Square const &from, Square const &to);
const Move composeMove(Square const &from, Square const &to, Color const &c, ushort const &p, MoveType const &type,
	Piece const &captured, ushort const &promoteTo);
short underCheck(Color const &c, Position const &p);
const std::vector<Move> pruneIllegal(std::vector<Move> &moveList, Position const &p);


// makemove.cpp
void doMove(Move const &m, Position &p);
void undoMove(Move const &m, Position &p, Position const &backup);


// perft.cpp
unsigned long long perft(short depth, Position& p, bool init = false);
template<size_t PERFT_CACHE_SIZE>
static unsigned long long perft(short depth, Position& p, std::array<perftCache, PERFT_CACHE_SIZE>& cache);
void perftInit();


// zobrist.cpp
namespace Zobrist {
	void init();
	uint64_t fill(Position const p);
	uint64_t getKey(Color const& c, Piece const& p, Square const& sq);
	uint64_t getKey(Square const& sq);
	uint64_t getKey(Color const& c);
	uint64_t getKey(Color const& c, Castle const& castle);
	uint64_t getKey(ushort depth);
};


// evaluation.cpp
const short evaluate(Position const &p);
const ushort evMaterial(Position const& p);
const short evPSQT(Position const& p);

// search.cpp

const Move calculateBestMove(Position const &p, short depth);


// bitscan.cpp
#if defined(__GNUC__)  // GCC, Clang, ICC
const ushort bitscan_fwd(Bitboard const &b);
const ushort bitscan_rvs(Bitboard const &b);
#elif defined(_MSC_VER)  // MSVC
#ifdef _WIN64  // MSVC, WIN64
const ushort bitscan_fwd(Bitboard const &b);
const ushort bitscan_rvs(Bitboard const &b);
#else  // MSVC, WIN32
const ushort bitscan_fwd(Bitboard const &b);
const ushort bitscan_rvs(Bitboard const &b);
#endif
#else  // Compiler is neither GCC nor MSVC compatible
#error "Compiler not supported."
#endif
const ushort popcount(Bitboard const &b);
const ushort bitscan_reset(Bitboard &b, bool reverse = 0);


// overloading.cpp
Square operator++(Square& s, int);
Square operator--(Square& s, int);
File operator++(File &f, int);
File operator--(File &f, int);
Rank operator++(Rank &r, int);
Rank operator--(Rank &r, int);
Piece operator++(Piece &p, int);
Color operator++(Color& c, int);


template<bool Root>
inline uint64_t divide(short depth, Position &p)
{
	std::vector<Move> moveList = moveGeneration(p);
	uint64_t cnt, nodes = 0;
	Position copy = p;
	moveList = pruneIllegal(moveList, copy);
	const bool leaf = (depth == 1);

	for (const auto& m : moveList)
	{
		if (Root && depth == 0)
			cnt = 1, nodes++;
		else
		{
			doMove(m, copy);
			cnt = leaf ? 1 : divide<false>(depth - 1, copy);
			nodes += cnt;
			undoMove(m, copy, p);
		}
		if (Root) {
			Square squareFrom{}, squareTo{};
			squareFrom = Square(((C64(1) << 6) - 1) & (m >> 19));
			squareTo = Square(((C64(1) << 6) - 1) & (m >> 13));
			ushort promotedTo = ((C64(1) << 3) - 1) & (m);
			if (promotedTo)
				switch (promotedTo) {
				case PAWN_TO_QUEEN:
					std::cout << "q";
					break;
				case PAWN_TO_KNIGHT:
					std::cout << "n";
					break;
				case PAWN_TO_ROOK:
					std::cout << "r";
					break;
				case PAWN_TO_BISHOP:
					std::cout << "b";
					break;
				}
			std::cout << squareToStringMap[squareFrom] << squareToStringMap[squareTo];
			std::cout << ": " << cnt << std::endl;
			}
	}
	return nodes;
}

#endif