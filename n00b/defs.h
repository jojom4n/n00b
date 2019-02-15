#pragma once
#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>
#include <string>
#include <array>
#include <map>

#define C64(constantuint64_t) constantuint64_t##ULL
#define FILE_INDEX (square % 8)
#define RANK_INDEX (square / 8)

typedef unsigned short int ushort;

typedef uint64_t Bitboard;

// ENUMS
enum Piece : const ushort { KING, QUEEN, ROOKS, KNIGHTS, BISHOPS, PAWNS, NO_PIECE };

enum Color : const ushort { BLACK, WHITE, ALL_COLOR };

enum Square : const ushort
{
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	SQ_NUMBER, SQ_EMPTY
};

extern std::map<std::string, Square> SquareMap;

/* std::map<Square std::string> SquareString = {
	{ A1,"a1" }, { B1,"b1" }, { C1,"c1" }, { D1,"d1"}, {E1,"e1" }, { F1,"f1"}, {G1,"g1"},{H1,"h1"},
	{ A2,"a2" }, { B2,"b2" }, { C2,"c2" }, { D2,"d2"}, {E2,"e2" }, { F2,"f2"}, {G2,"g2"},{H2,"h2"},
	{ A3,"a3" }, { B3,"b3" }, { C3,"c3" }, { D3,"d3"}, {E3,"e3" }, { F3,"f3"}, {G3,"g3"},{H3,"h3"},
	{ A4,"a4" }, { B4,"b4" }, { C4,"c4" }, { D4,"d4"}, {E4,"e4" }, { F4,"f4"}, {G4,"g4"},{H4,"h4"},
	{ A5,"a5" }, { B5,"b5" }, { C5,"c5" }, { D5,"d5"}, {E5,"e5" }, { F5,"f5"}, {G5,"g5"},{H5,"h5"},
	{ A6,"a6" }, { B6,"b6" }, { C6,"c6" }, { D6,"d6"}, {E6,"e6" }, { F6,"f6"}, {G6,"g6"},{H6,"h6"},
	{ A7,"a7" }, { B7,"b7" }, { C7,"c7" }, { D7,"d7"}, {E7,"e7" }, { F7,"f7"}, {G7,"g7"},{H7,"h7"},
	{ A8,"a8" }, { B8,"b8" }, { C8,"c8" }, { D8,"d8"}, {E8,"e8" }, { F8,"f8"}, {G8,"g8"},{H8,"h8"}
}; */

enum Castle : const ushort { KINGSIDE = 1, QUEENSIDE, ALL, NONE = 0 };

enum File : const ushort { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NUMBER };

enum Rank : const ushort { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NUMBER };

enum Rays : const ushort {NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST, RAYS_NUMBER};

enum RaysKnight : const ushort {NORTH_NORTH_WEST, NORTH_NORTH_EAST, NORTH_EAST_EAST, SOUTH__EAST_EAST, SOUTH_SOUTH_EAST,
	SOUTH_SOUTH_WEST, SOUTH__WEST_WEST, NORTH_WEST_WEST, RAYS_KNIGHT_NUMBER};


// OTHER TYPES
struct coords { ushort x; ushort y; };

// see magic.cpp
constexpr ushort ROOK_INDEX_BITS = 12, BISHOP_INDEX_BITS = 9;

constexpr uint64_t A1H8_DIAG = C64(0x8040201008040201);
constexpr ushort DIAG_NUMBER = 15;

constexpr uint64_t NOT_FILE_A = 0xfefefefefefefefe;
constexpr uint64_t NOT_FILE_H = 0x7f7f7f7f7f7f7f7f;
constexpr uint64_t NOT_FILE_AB = 0xfcfcfcfcfcfcfcfc;
constexpr uint64_t NOT_FILE_GH = 0x3f3f3f3f3f3f3f3f;


// for popcount() functions - see https://www.chessprogramming.org/Population_Count
const Bitboard k1 = C64(0x5555555555555555); /*  -1/3   */
const Bitboard k2 = C64(0x3333333333333333); /*  -1/5   */
const Bitboard k4 = C64(0x0f0f0f0f0f0f0f0f); /*  -1/17  */
const Bitboard kf = C64(0x0101010101010101); /*  -1/255 */

struct Mask {
	std::array<Bitboard, FILE_NUMBER> file;
	std::array<Bitboard, RANK_NUMBER> rank;
	std::array<Bitboard, DIAG_NUMBER> diagonal;
	std::array<Bitboard, DIAG_NUMBER> antiDiagonal;
	std::array<Bitboard, SQ_NUMBER> linesEx;
	std::array<Bitboard, SQ_NUMBER> diagonalsEx;
	std::array<std::array<Bitboard, SQ_NUMBER>, RAYS_NUMBER>rays;
	std::array<std::array<Bitboard,SQ_NUMBER>, RAYS_NUMBER>raysEx;
};

struct AttackTable {
	std::array<Bitboard, SQ_NUMBER> king;
	std::array<Bitboard, SQ_NUMBER> knight;
	std::array<std::array<Bitboard, 1 << ROOK_INDEX_BITS>, SQ_NUMBER> rookMagic;
	std::array<std::array<Bitboard, 1 << BISHOP_INDEX_BITS>, SQ_NUMBER> bishopMagic;
	const Bitboard whitePawn(Bitboard const &pawn, Bitboard const &occupancy) const;
	const Bitboard blackPawn(Bitboard const &pawn, Bitboard const &occupancy) const;
	const Bitboard rook(Square const &square, Bitboard const &blockers) const;
	const Bitboard bishop(Square const &square, Bitboard const &blockers) const;
};

extern const std::array<std::string, 65> squares_to_string;
extern const Bitboard MAGIC_ROOK[64];
extern const Bitboard MAGIC_BISHOP[64];
extern const ushort SHIFT_ROOK[64];
extern const ushort SHIFT_BISHOP[64];

#endif