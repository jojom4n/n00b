#include "pch.h"
#include <iostream>
#include <sstream>
#include "protos.h"
#include "Position.h"

std::string input = " ";

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
		std::vector<Move> moveList;
		moveList = moveGeneration(board);
		if (moveList.size() > 0) {
			displayMoveList(board, moveList);
		}
		else
		{
			board.setCheckmate(true);
			std::cout << "It's CHECKMATE!" << std::endl;
		}
	}

	else if ((inputStream.str().substr(0, 12) == "display" && numWords == 1))
		displayBoard(board);
	
	else if (inputStream.str().substr(0, 4) == "quit" && numWords == 1)
		return;
	
	else if (inputStream.str().substr(0, 3) == "new" && numWords == 1) {
		delete &board;
		newGame();
	}
	
	else if (inputStream.str().substr(0, 6) == "search" && numWords == 1) {
		Move m = calculateBestMove(board, 3);
		if (m) {
			doMove(m, board);
			displayBoard(board);
		}
		else
		{
			board.setCheckmate(true);
			std::cout << "It's CHECKMATE!" << std::endl;
		}	
	}
	else if (inputStream.str().substr(0, 5) == "perft" && numWords == 2) {
		short depth = stoi(inputStream.str().substr(6, 1));
		if (depth > 0) {
			auto t1 = Clock::now();
			std::cout << perft(depth, board) << std::endl;
			auto t2 = Clock::now();
			std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
		}
		else
			std::cout << "Invalid depth.\n";
	}
	else if (inputStream.str().substr(0, 6) == "divide" && numWords == 2) {
		short depth = stoi(inputStream.str().substr(7, 1));
		if (depth > 0) {
			auto t1 = Clock::now();
			std::cout << divide<true>(depth, board) << std::endl;
			auto t2 = Clock::now();
			std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
		}
		else
			std::cout << "Invalid depth.\n";
	}
	else
		std::cout << "Invalid command.\n";
}