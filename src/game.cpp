#include "pch.h"
#include "game.h"
#include "display.h"
#include "divide.h"
#include "evaluation.h"
#include "fen.h"
#include "makemove.h"
#include "movegen.h"
#include "perft.h"
#include "Position.h"
#include "search.h"
#include <iostream>

std::string input = " ";

extern unsigned long long debug_nodes;

void newGame()
{	
	Position Chessboard;
	Chessboard.setNew();
	displayBoard(Chessboard);
	
	do 
	{
		(Chessboard.getTurn() == WHITE) ? 
			std::cout << "\nwhite>> " : std::cout << "\nblack>> ";

		std::getline(std::cin, input);
		std::stringstream lineStream(input);

		readCommand(lineStream, Chessboard);

		std::cout << std::endl;
	} while (input != "quit");

}


void readCommand(std::stringstream &inputStream, Position &board)
{
	int numWords = 0;
	
	while (inputStream >> input) numWords++; // count the words in the input stream

	if (inputStream.str().substr(0,12) == "position fen" && inputStream.str().substr(12,1) == " ")
		if (numWords >= 3 && fenValidate(inputStream))
			fenParser(inputStream, board);
		else std::cout << "Sorry, no FEN or invalid FEN position.\n";
	
	else if (inputStream.str().substr(0) == "movelist") {
		std::vector<Move> moveList = moveGeneration(board);
		pruneIllegal(moveList, board);
		
		if (moveList.size() > 0) {
			displayMoveList(board, moveList);
		}
		else if (moveList.size() == 0 && !underCheck(board.getTurn(), board)) {
			std::cout << "It's STALEMATE!" << std::endl;
		}
		else if (moveList.size() == 0 && underCheck(board.getTurn(), board)) {
			board.setCheckmate(true);
			std::cout << "It's CHECKMATE!" << std::endl;
		}
	}

	else if ((inputStream.str().substr(0) == "display"))
		displayBoard(board);
	
	else if (inputStream.str().substr(0) == "quit")
		return;
	
	else if (inputStream.str().substr(0) == "new") {
		newGame();
	}
	
	else if (inputStream.str().substr(0, 6) == "search" && inputStream.str().substr(6,1) == " "
		&& numWords == 2) {
		short depth = stoi(inputStream.str().substr(7));
		
		if (depth > 0 && !(depth > MAX_PLY)) {
			Move m = iterativeSearch(board, depth);
			if (m) {
				doMove(m, board);
				std::cout << std::endl;
				displayBoard(board);
			}
		}
		else
			std::cout << "Invalid depth.\n";
	}

	else if (inputStream.str().substr(0, 5) == "perft" && inputStream.str().substr(5, 1) == " "
		&& numWords == 2) {
		short depth = stoi(inputStream.str().substr(6));
		if (depth > 0 && !(depth > MAX_PLY)) {
			unsigned long long nodes = 0;
			const auto t1 = Clock::now();
			nodes = perft(depth, board);
			const auto t2 = Clock::now();
			std::cout << "Total moves: " << nodes;
			std::cout << "\tTime elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms\n";;
			std::cout << "Total mg() generations: " << debug_nodes << std::endl;
			debug_nodes = 0; // reset for another eventual iteration
		}
		else
			std::cout << "Invalid depth.\n";
	}

	else if (inputStream.str().substr(0, 6) == "divide" && inputStream.str().substr(6, 1) == " " 
		&& numWords == 2) {
		short depth = stoi(inputStream.str().substr(7));
		if (depth > 0 && !(depth > MAX_PLY)) {
			auto t1 = Clock::now();
			std::cout << divide<true>(depth, board) << "\n";
			auto t2 = Clock::now();
			std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms\n";
		}
		else
			std::cout << "Invalid depth.\n";
	}

	else if ((inputStream.str().substr(0) == "eval")) {
		std::cout << "Material evaluation is: " << evMaterial(board) << "\n";
		std::cout << "PSQT evaluation is: " << evPSQT(board);
	}
	
	else
		std::cout << "Invalid command.\n";
}

