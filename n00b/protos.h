#pragma once
#ifndef PROTOS_H
#define PROTOS_H

#include <string>
#include "defs.h"
#include "Position.h"

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
const Bitboard gen_r_attks(ushort const &square, Bitboard const &blockerboard);
const Bitboard gen_b_attks(ushort const &square, Bitboard const &blockerboard);
void rookMagic();
void bishopMagic();

// game.cpp
void newGame();
void readCommand(std::stringstream &inputStream, Position &board);

// fen.cpp
bool fenValidate(const std::stringstream &fen);
void fenParser(std::stringstream &fen, Position &board);

//display.cpp
void display_board(Position const &board);
const char printPiece(coords const &coordinates);

// movegen.cpp
void moveGeneration(Position const &board);
void updateMoveList(Square const &squareFrom, Square const &squareTo,
	MoveType const &type, PromotionTo const &promoteTo = PAWN_TO_QUEEN);

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

#endif