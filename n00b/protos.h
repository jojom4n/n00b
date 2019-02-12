#pragma once
#ifndef PROTOS_H
#define PROTOS_H

#include <string>
#include "defs.h"
#include "Board.h"

// attack.cpp
void initAttacks();
void generateRookMask(struct Mask Masks);
void lineAttacks();
void rayAttacks();

// magic.cpp
const Bitboard rook_attack(Square const &square, Bitboard const &blockers);
const Bitboard gen_blockerboard(int index, ushort bits, Bitboard b);
const Bitboard gen_r_attks(ushort square, Bitboard b);
void rookMagic();

// game.cpp
void newGame();

//display.cpp
const std::string display_board(Board const &board);
const char printPiece(coords const &coordinates);

// bitscan.cpp
#if defined(__GNUC__)  // GCC, Clang, ICC
const Square bitscan_fwd(Bitboard const &b);
const Square bitscan_rvs(Bitboard const &b);
#elif defined(_MSC_VER)  // MSVC
#ifdef _WIN64  // MSVC, WIN64
const Square bitscan_fwd(Bitboard const &b);
const Square bitscan_rvs(Bitboard const &b);
#else  // MSVC, WIN32
const Square bitscan_fwd(Bitboard const &b);
const Square bitscan_rvs(Bitboard const &b);
#endif
#else  // Compiler is neither GCC nor MSVC compatible
#error "Compiler not supported."
#endif

const ushort popcount(Bitboard const &b);

const Square bitscan_reset(Bitboard &b);

#endif