#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include "Position.h"
#include "defs.h"
#include "protos.h"

std::string input = " ";
extern struct Mask Masks;
extern struct LookupTable MoveTables;
extern std::list<Move> moveList;
extern std::map<std::string, Square> StringToSquareMap;
extern std::map<Square, std::string> SquareToStringMap;

void newGame()
{	
	Position *Chessboard = new Position();
	Chessboard->setNew();
	displayBoard(*Chessboard);
	
	do 
	{
		(Chessboard->getTurn() == WHITE) ? 
			std::cout << "\nwhite>> " : std::cout << "\nblack>> ";

		std::getline(std::cin, input);
		std::stringstream lineStream(input);

		readCommand(lineStream, *Chessboard);

		std::cout << std::endl;
	} while (input != "quit");
}

void readCommand(std::stringstream &inputStream, Position &board)
{
	int numWords = 0;

	while (inputStream >> input) numWords++; // count the words in the input stream
	
	if (inputStream.str().substr(0, 12) == "fen position")
		if (numWords >= 3 && fenValidate(inputStream))
			fenParser(inputStream, board);
		else std::cout << "Sorry, no FEN or invalid FEN position.\n";
	else if (inputStream.str().substr(0, 8) == "movelist" && numWords == 1) {
		moveGeneration(board);
		displayMoveList(board);
	}
	else if ((inputStream.str().substr(0, 12) == "display" && numWords == 1))
		displayBoard(board);
	else if (inputStream.str().substr(0, 4) == "quit" && numWords == 1)
		return;
	else if (inputStream.str().substr(0, 3) == "new" && numWords == 1) {
		delete &board;
		newGame();
	}
	else
		std::cout << "Invalid command.\n";
}

