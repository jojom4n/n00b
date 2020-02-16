#ifndef ENUM_H
#define ENUM_H

#include "defs.h"

enum Piece : const short { KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN, NO_PIECE };
enum Color : const ushort { BLACK, WHITE, ALL_COLOR };

enum Square : const short
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

enum MoveType : const ushort { QUIET = 1, CAPTURE, EVASION, PROMOTION, CASTLE_K, CASTLE_Q, EN_PASSANT };
enum { PAWN_TO_QUEEN = 1, PAWN_TO_KNIGHT, PAWN_TO_ROOK, PAWN_TO_BISHOP };
enum Castle : const ushort { KINGSIDE = 1, QUEENSIDE, ALL, NONE = 0 };
enum File : const short { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NUMBER };
enum Rank : const short { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NUMBER };
enum Rays : const ushort { NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST, RAYS_NUMBER };

enum RaysKnight : const ushort {
	NORTH_NORTH_WEST, NORTH_NORTH_EAST, NORTH_EAST_EAST, SOUTH__EAST_EAST, SOUTH_SOUTH_EAST,
	SOUTH_SOUTH_WEST, SOUTH__WEST_WEST, NORTH_WEST_WEST, RAYS_KNIGHT_NUMBER
};

enum nodeType : const char { EXACT, LOWER_BOUND, UPPER_BOUND };

struct PieceID { Color color; Piece piece; };

#endif
