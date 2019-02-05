#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <iostream>
#include <string>

using std::string;

class Board
{
	// let's define the bitboards...
	typedef uint64_t bitboard;
	
	// ...and let's create those bitboards;
	bitboard W_King, W_Queen, W_Pawns, W_Knights, W_Bishops, W_Rooks,
		B_King, B_Queen, B_Pawns, B_Knights, B_Bishops, B_Rooks;

public:
	Board();
	~Board();
	void init(const string fen);
};

#endif