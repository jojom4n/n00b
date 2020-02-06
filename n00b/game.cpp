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


template<bool Root>
uint64_t divide(short depth, Position& p)
{
	std::vector<Move> moveList = moveGeneration(p);
	uint64_t cnt, nodes = 0;
	Position copy = p;
	moveList = pruneIllegal(moveList, copy);
	const bool leaf = (depth == 1);

	for (const auto& m : moveList)
	{
		if (Root && depth == 0)
			cnt = 1, nodes++;
		else
		{
			doMove(m, copy);
			cnt = leaf ? 1 : divide<false>(depth - 1, copy);
			nodes += cnt;
			undoMove(m, copy, p);
		}
		if (Root) {
			Square squareFrom{}, squareTo{};
			squareFrom = Square(((C64(1) << 6) - 1) & (m >> 19));
			squareTo = Square(((C64(1) << 6) - 1) & (m >> 13));
			ushort promotedTo = ((C64(1) << 3) - 1) & (m);
			if (promotedTo)
				switch (promotedTo) {
				case PAWN_TO_QUEEN:
					std::cout << "q";
					break;
				case PAWN_TO_KNIGHT:
					std::cout << "n";
					break;
				case PAWN_TO_ROOK:
					std::cout << "r";
					break;
				case PAWN_TO_BISHOP:
					std::cout << "b";
					break;
				}
			std::cout << squareToStringMap[squareFrom] << squareToStringMap[squareTo];
			std::cout << ": " << cnt << std::endl;
		}
	}
	return nodes;
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
		else if (moveList.size() == 0 && !underCheck(board.getTurn(), board))
		{
			std::cout << "It's STALEMATE!" << std::endl;
		}
		else if (moveList.size() == 0 && underCheck(board.getTurn(), board))
		{
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

		Move m = calculateBestMove(board, depth, true);

		if (m) {
			doMove(m, board);
			displayBoard(board);
			displayMove(board, m);
		}
		else if (!m && !underCheck(board.getTurn(), board))
		{
			std::cout << "It's STALEMATE!" << std::endl;
		}	
		else if (!m && underCheck(board.getTurn(), board))
		{
			board.setCheckmate(true);
			std::cout << "It's CHECKMATE!" << std::endl;
		}
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
	else
		std::cout << "Invalid command.\n";
}

