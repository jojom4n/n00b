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
const Bitboard gen_r_attks(Square const &square, Bitboard const &blockerboard);
const Bitboard gen_b_attks(Square const &square, Bitboard const &blockerboard);
void rookMagic();
void bishopMagic();


// game.cpp
void newGame();
void readCommand(std::stringstream &inputStream, Position &board);


// fen.cpp
const bool fenValidate(std::stringstream &fen);
void fenParser(std::stringstream &fen, Position &board);


//display.cpp
void displayBoard(Position const &board);
void displayMoveList(Position const &board, std::vector<Move> const &m);
const char printPiece(PieceID const &ID);


// movegen.cpp
const std::vector<Move> moveGeneration(Position const &p);
const std::vector<Move> generateOnlyKing(Color const &c, Position const &p);
const Bitboard pawnMoves(Color const &c, Square const &from, Bitboard const &occ, Bitboard const &own);
void castleMoves(Position const &p, Check const &isCheck);
void enPassant(Position const &p, Check const &isCheck);
const Check isChecking(Piece const &piece, Square const &squareTo, Position const &board);
const MoveType setType(Piece const &piece, Bitboard const &m, Position const &p, Square const &from, Square const &to);
const Move composeMove(Square const &from, Square const &to, Color const &c, ushort const &p, MoveType const &type,
	Piece const &captured, ushort const &promoteTo, Check const &check);
short underCheck(Color const &c, Position const &p);
const std::vector<Move> pruneIllegal(std::vector<Move> &moveList, Position const &p);


// makemove.cpp
void doMove(Move const &m, Position &p);
void undoMove(Move const &m, Position &p, Position const &backup);


// perft.cpp
unsigned long long perft(short depth, Position const &p);
unsigned long long divide(short depth, Position const &p);

// evaluation.cpp
const short evaluate(Position const &pos);
const short evMaterial(Position const &pos, Color const &color);
const short evPSQT(Position const &p, Color const &color);


// search.cpp
const short negamax(Position &p, short depth, short alpha, short beta);
const Move calculateBestMove(Position &p, short depth);


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


// overloading.cpp
Square operator++(Square& s, int);
Square operator--(Square& s, int);
File operator++(File &f, int);
File operator--(File &f, int);
Rank operator++(Rank &r, int);
Rank operator--(Rank &r, int);
Piece operator++(Piece &p, int);



#endif