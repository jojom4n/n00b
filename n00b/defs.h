#pragma once
#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>
#include <string>
#include <array>

#define C64(constantU64) constantU64##ULL
#define FILE_INDEX (square % 8)
#define RANK_INDEX (square / 8)

typedef unsigned short int ushort;

typedef uint64_t Bitboard;

// ENUMS
enum Piece : const ushort { KING, QUEEN, ROOKS, KNIGHTS, BISHOPS, PAWNS, NO_PIECE };

enum Color : const ushort { BLACK, WHITE, NO_COLOR };

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

enum File : const ushort { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NUMBER };

enum Rank : const ushort { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NUMBER };

enum Rays : const ushort {NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST, RAYS_NUMBER};

// OTHER TYPES
struct coords { ushort x; ushort y; };

// see magic.cpp
constexpr ushort ROOK_INDEX_BITS = 12, BISHOP_INDEX_BITS = 9;

constexpr uint64_t A1H8_DIAG = C64(0x8040201008040201);
constexpr ushort DIAG_NUMBER = 15;
constexpr uint64_t NOT_FILE_A = 0xfefefefefefefefe;

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
	std::array<Bitboard, SQ_NUMBER> rook;
	std::array<std::array<Bitboard,SQ_NUMBER>, RAYS_NUMBER>raysNoEdge;
};

struct AttackTables {
	std::array<std::array<Bitboard, 1 << ROOK_INDEX_BITS>, SQ_NUMBER> rook;
	std::array<std::array<Bitboard, 1 << BISHOP_INDEX_BITS>, SQ_NUMBER> bishop;
};

extern const std::array<std::string, 65> squares_to_string;

#endif