#include "pch.h"
#include <iostream>
#include <bitset>
#include "Board.h"
#include "defs.h"
#include "protos.h"

std::string input = " ";
extern struct Mask Masks;
extern struct AttackTable Attacks;

void newGame()
{	
	Board *Chessboard = new Board();
	Chessboard->setNew();
	std::cout << std::endl << display_board(*Chessboard) << std::endl;
	
	// TEST MAGIC
	U64 x = getRookAttacks(F6,Chessboard->getAllBlockers());
	std::cout << "\n\nMagic bitboard for Rook on F6: " << x << std::endl;
	std::cout << std::endl;
	//END TEST

	// TEST MAGIC
	std::cout << "\n\nMask bitboard for Rook on G5: " << Masks.linesEx[G5] << std::endl;
	std::cout << std::endl;
	//END TEST

	// TEST MAGIC
	x = getBishopAttacks(D3, Chessboard->getAllBlockers());
	std::cout << "\n\nMagic bitboard for Bishop on D3: " << x << std::endl;
	std::cout << std::endl;
	//END TEST

	std::cout << "\n\nMask bitboard for Bishop on E4: " << Masks.diagonalsEx[E4] << std::endl;

	std::cout << sizeof(Masks.diagonal);


	for (;;)
	{
		(Chessboard->getMove() == WHITE) ? 
			std::cout << "\nwhite>> " : std::cout << "\nblack>> ";

		std::cin >> input;
	}
}