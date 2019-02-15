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

