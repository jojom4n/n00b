#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>

// definitions for bitboard "bb" (see Board.h)
typedef uint64_t Bitboard;

enum Piece {king, queen, rooks, knights, bishops, pawns};

enum Color {white, black};

enum Castle { castle_kingside, castle_queenside, castle_both }; // for castling - see Board.h

enum Square : int
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

enum Option { quit, help, fen, play, sysinfo, display, invalid };

enum File { file_A, file_B, file_C, file_D, file_E, file_F, file_G, file_H};

enum Rank {rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8};
#endif
