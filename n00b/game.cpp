#include "pch.h"
#include <iostream>
#include <sstream>
#include "protos.h"
#include "Position.h"
#include "divide.h"

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

	if (inputStream.str().substr(0,12) == "position fen" && inputStream.str().substr(12,1) == " ")
		if (numWords >= 3 && fenValidate(inputStream))
			fenParser(inputStream, board);
		else std::cout << "Sorry, no FEN or invalid FEN position.\n";
	
	else if (inputStream.str().substr(0) == "movelist") {
		std::vector<Move> moveList;
		moveList = moveGeneration(board);
		moveList = pruneIllegal(moveList, board);
		
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
		delete &board;
		newGame();
	}
	
	else if (inputStream.str().substr(0, 6) == "search" && inputStream.str().substr(6,1) == " "
		&& numWords == 2) {
		short depth = stoi(inputStream.str().substr(7));
		long nodes{};
		Move m{};
		std::cout << std::endl;

		for (short i = 1; i <= depth; i++) {
			short bestScore = -SHRT_INFINITY;
			std::vector<Move> pv{};
			pv.clear();
			
			auto t1 = Clock::now();
			m = searchRoot(board, i, bestScore, nodes, pv);
			auto t2 = Clock::now();

			std::chrono::duration<float, std::milli> time = t2 - t1;

			if (m) {
				std::cout << "depth:" << i << "\tnodes:" << nodes << " ms:" << time.count() << " nps:" << nodes / (time.count() / 1000) << std::endl;  
				std::cout << "move:" << displayMove(board, m) << " score:" << bestScore << " pv:";

				for (auto &m : pv) {
					std::cout << displayMove(board, m) << " ";
				}

				std::cout << std::endl << std::endl;
			}

			else if (!m && !underCheck(board.getTurn(), board)) {
				std::cout << "\nIt's STALEMATE!" << std::endl;
			}

			else if (!m && underCheck(board.getTurn(), board)) {
				board.setCheckmate(true);
				std::cout << "\nIt's CHECKMATE!" << std::endl;
			}
		}

		doMove(m, board);
		std:: cout << std::endl;
		displayBoard(board);
	}

	else if (inputStream.str().substr(0, 5) == "perft" && inputStream.str().substr(5, 1) == " "
		&& numWords == 2) {
		short depth = stoi(inputStream.str().substr(6));
		if (depth > 0) {
			auto t1 = Clock::now();
			std::cout << perft(depth, board) << std::endl;
			auto t2 = Clock::now();
			std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
		}
		else
			std::cout << "Invalid depth.\n";
	}

	else if (inputStream.str().substr(0, 6) == "divide" && inputStream.str().substr(6, 1) == " " 
		&& numWords == 2) {
		short depth = stoi(inputStream.str().substr(7));
		if (depth > 0) {
			auto t1 = Clock::now();
			std::cout << divide<true>(depth, board) << std::endl;
			auto t2 = Clock::now();
			std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
		}
		else
			std::cout << "Invalid depth.\n";
	}

	else if ((inputStream.str().substr(0) == "eval")) {
		std::cout << "Material evaluation is: " << evMaterial(board) << std::endl;
		std::cout << "PSQT evaluation is: " << evPSQT(board);
	}
	
	else
		std::cout << "Invalid command.\n";
}

