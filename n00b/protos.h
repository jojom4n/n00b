#pragma once
#ifndef PROTOS_H
#define PROTOS_H

#include <string>
#include "defs.h"
#include "Position.h"
#include "Evaluation.h"


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
bool fenValidate(std::stringstream &fen);
void fenParser(std::stringstream &fen, Position &board);


//display.cpp
void displayBoard(Position const &board);
void displayMoveList(Position const &board, std::vector<Move> const &m);
const char printPiece(PieceID const &ID);


// movegen.cpp
std::vector<Move> moveGeneration(Position const &board);
Check verifyCheck(Piece const &piece, Square const &squareTo, Position const &board);
Move formMove(Square const &squareFrom, Square const &squareTo, Color const &color, 
	ushort const &piece, MoveType const &type, Piece const &captured, bool const &promoteTo, Check const &check);
void doMove(Move const m, Position &p);
void undoMove(Move const m, Position &p, Position const &copy);


// Evaluation.cpp
int evaluate(Position const &pos);
int evMaterial(Position const &pos);
int evPSQT(Position const &p);


// search.cpp
short negamax(Position p, short depth);
constexpr short max(short const &x, short const &y) { return (x > y) ? x : y; }


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