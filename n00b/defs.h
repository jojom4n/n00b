#ifndef DEFS_H
#define DEFS_H

// definitions for bitboard "bb" (see Board.h)
constexpr bool white = 0;
constexpr bool black = 1;
constexpr unsigned short king = 0;
constexpr unsigned short queen = 1;
constexpr unsigned short rooks = 2;
constexpr unsigned short knights = 3;
constexpr unsigned short bishops = 4;
constexpr unsigned short pawns = 5;

// definitions for commands.cpp
enum Options { quit, help, fen, play, sysinfo, display, invalid };

#endif
