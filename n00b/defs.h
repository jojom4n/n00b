#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>
#include <string>

#define C64(constantU64) constantU64##ULL

typedef unsigned short int ushort;

// definitions for bitboard
typedef uint64_t Bitboard;

struct bb_index
{
	ushort x;
	ushort y;
};

// for popcount() function in Board class - see https://www.chessprogramming.org/Population_Count
const Bitboard k1 = C64(0x5555555555555555); /*  -1/3   */
const Bitboard k2 = C64(0x3333333333333333); /*  -1/5   */
const Bitboard k4 = C64(0x0f0f0f0f0f0f0f0f); /*  -1/17  */
const Bitboard kf = C64(0x0101010101010101); /*  -1/255 */

enum Piece : ushort { king, queen, rooks, knights, bishops, pawns, no_piece };

enum Color : ushort { black, white, no_color };

enum Square : ushort
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

extern const std::string squares_to_string[65];

enum File : ushort { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NUMBER};

enum Rank : ushort{ RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NUMBER};

extern Bitboard rank_mask[RANK_NUMBER], file_mask[FILE_NUMBER];

extern Bitboard north_attack[SQ_NUMBER], south_attack[SQ_NUMBER], east_attack[SQ_NUMBER], west_attack[SQ_NUMBER];
#endif
