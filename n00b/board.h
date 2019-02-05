#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <iostream>
#include <string>

using std::string;

const string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

class Board
{
	// let's define the bitboards...
	typedef uint64_t bitboard;
	
	enum Square {
		a1, b1, c1, d1, e1, f1, g1, h1,
		a2, b2, c2, d2, e2, f2, g2, h2,
		a3, b3, c3, d3, e3, f3, g3, h3,
		a4, b4, c4, d4, e4, f4, g4, h4,
		a5, b5, c5, d5, e5, f5, g5, h5,
		a6, b6, c6, d6, e6, f6, g6, h6,
		a7, b7, c7, d7, e7, f7, g7, h7,
		a8, b8, c8, d8, e8, f8, g8, h8
	};

	// ...and let's create those bitboards;
	bitboard W_King, W_Queen, W_Pawns, W_Knights, W_Bishops, W_Rooks,
		B_King, B_Queen, B_Pawns, B_Knights, B_Bishops, B_Rooks;

public:
	Board();
	~Board();
	void init(const string fen = startFEN);
};

#endif