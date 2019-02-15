#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
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
	display_board(*Chessboard);
	
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

		std::getline(std::cin, input);
		std::stringstream lineStream(input);

		readCommand(lineStream, *Chessboard);

		std::cout << std::endl;
	}
}

void readCommand(std::stringstream &inputStream, Position &board)
{
	int numWords = 0;

	while (inputStream >> input) numWords++; // count the words in the input stream
	
	if (inputStream.str().substr(0,12) == "fen position")
		if (numWords >= 3 && fenValidate(inputStream))
			fenParser(inputStream, board);
		else std::cout << "Sorry, no FEN or invalid FEN position.\n";
	else
		std::cout << "Invalid command.\n";
}

