#ifndef DEFS_H
#define DEFS_H

// definitions for bitboard "bb" (see Board.h)
constexpr bool white = 0;
constexpr bool black = 1;

constexpr unsigned short W_King = 0;
constexpr unsigned short W_Queen = 1;
constexpr unsigned short W_Rooks = 2;
constexpr unsigned short W_Knights = 3;
constexpr unsigned short W_Bishops = 4;
constexpr unsigned short W_Pawns = 5;
constexpr unsigned short B_King = 6;
constexpr unsigned short B_Queen = 7;
constexpr unsigned short B_Rooks = 8;
constexpr unsigned short B_Knights = 9;
constexpr unsigned short B_Bishops = 10;
constexpr unsigned short B_Pawns = 11;

// definitions for commands.cpp
enum Options { quit, help, fen, play, sysinfo, display, invalid };

#endif
