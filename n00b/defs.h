#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>

typedef unsigned short int ushort;

// definitions for bitboard "bb" (see Board.h)
typedef uint64_t Bitboard;

struct bb_coordinates
{
	ushort x;
	ushort y;
};

enum Piece : ushort {king, queen, rooks, knights, bishops, pawns};

enum Color : bool {white, black};

enum Castle : ushort { castle_kingside, castle_queenside, castle_both };

enum Square : ushort
{
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8
};

enum Option : ushort { quit, help, fen, play, sysinfo, print, invalid };

enum File : ushort { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};

enum Rank : ushort{ RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};

#endif
