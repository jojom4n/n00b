#ifndef PROTOS_H
#define PROTOS_H

#include <string>
#include "defs.h"
#include "Board.h"

// tables.cpp
void init_tables();
void create_rook_mask();
void create_bishop_mask();
void create_file_mask();
void create_rank_mask();
void create_attacks_from();

// magic.cpp
const Bitboard rook_attack(Square const &square, Bitboard blockers);
void create_rook_magic();
const Bitboard gen_blockerboard(int index, Bitboard const &blockermask);

// game.cpp
void new_game();

//display.cpp
const std::string display_board(Board const &board);
const char print_piece(bb_index const &coordinates);

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

const ushort popcnt(Bitboard const &b);

const ushort bitscan_reset(Bitboard &b);

#endif