#ifndef MAGIC_H
#define MAGIC_H

#include "defs.h"
#include "enums.h"

constexpr ushort ROOK_INDEX_BITS = 12, BISHOP_INDEX_BITS = 9;
constexpr uint64_t A1H8_DIAG = C64(0x8040201008040201);
constexpr ushort DIAG_NUMBER = 15;
constexpr uint64_t NOT_FILE_A = 0xfefefefefefefefe;
constexpr uint64_t NOT_FILE_H = 0x7f7f7f7f7f7f7f7f;
constexpr uint64_t NOT_FILE_AB = 0xfcfcfcfcfcfcfcfc;
constexpr uint64_t NOT_FILE_GH = 0x3f3f3f3f3f3f3f3f;
constexpr uint64_t BB_RANK4 = 0xff000000;
constexpr uint64_t BB_RANK5 = 0x1095216660480;
const Bitboard gen_blockerboard(ushort const& index, ushort const& bits, Bitboard b);
const Bitboard gen_r_attks(Square const& square, Bitboard const& blockerboard);
const Bitboard gen_b_attks(Square const& square, Bitboard const& blockerboard);
extern const ushort SHIFT_ROOK[64];
extern const ushort SHIFT_BISHOP[64];
extern const Bitboard MAGIC_ROOK[64];
extern const Bitboard MAGIC_BISHOP[64];


void rookMagic();
void bishopMagic();

#endif
