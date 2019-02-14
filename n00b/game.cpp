#include "pch.h"
#include <iostream>
#include "Position.h"
#include "defs.h"
#include "protos.h"

std::string input = " ";
extern struct Mask Masks;
extern struct AttackTable Attacks;

void newGame()
{	
	Position *Chessboard = new Position();
	Chessboard->setNew();
	std::cout << std::endl << display_board(*Chessboard) << std::endl;
	
	// TEST MAGIC
	uint64_t x = Attacks.rook(A1, Chessboard->getAllBlockers());
	std::cout << "\n\nMagic bitboard for Rook on A1: " << x << std::endl;
	std::cout << std::endl;
	//END TEST

	// TEST MAGIC
	std::cout << "\n\nMask bitboard for Rook on G5: " << Masks.linesEx[G5] << std::endl;
	std::cout << std::endl;
	//END TEST

	// TEST MAGIC
	x = Attacks.bishop(F5, Chessboard->getAllBlockers());
	std::cout << "\n\nMagic bitboard for Bishop on F5: " << x << std::endl;
	std::cout << std::endl;
	//END TEST

	std::cout << "\n\nMask bitboard for Bishop on F6: " << Masks.diagonalsEx[F6] << std::endl;

	std::cout << "\n\nWhite pawns attacks: " << Attacks.whitePawn(Chessboard->getPieces(WHITE, PAWNS),
		C64(0xfff700000020ffff)) << std::endl;

	std::cout << std::endl << sizeof(Masks.diagonal);


	for (;;)
	{
		(Chessboard->getTurn() == WHITE) ? 
			std::cout << "\nwhite>> " : std::cout << "\nblack>> ";

		std::cin >> input;
	}
}