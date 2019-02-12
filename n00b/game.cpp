#include "pch.h"
#include <iostream>
#include <bitset>
#include "Board.h"
#include "defs.h"
#include "protos.h"

std::string input = " ";
extern struct Mask Masks;

void newGame()
{	
	Board *Chessboard = new Board();
	Chessboard->setNew();
	std::cout << std::endl << display_board(*Chessboard) << std::endl;
	
	// TEST MAGIC
	std::bitset<64>x(rook_attack(A1,Chessboard->getOpponentBlockers()));
	std::cout << "\n\nMagic bitboard for Rook on A1: " << x << std::endl;
	std::cout << std::endl;
	//END TEST

	// TEST MAGIC
	std::bitset<64>y(Masks.rook[B2]);
	std::cout << "\n\nMask bitboard for Rook on B2: " << y << std::endl;
	std::cout << std::endl;
	//END TEST

	for (;;)
	{
		(Chessboard->getMove() == WHITE) ? 
			std::cout << "\nwhite>> " : std::cout << "\nblack>> ";

		std::cin >> input;
	}
}