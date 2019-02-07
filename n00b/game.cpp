#include "pch.h"
#include <iostream>
#include <bitset>
#include "Board.h"
#include "defs.h"
#include "protos.h"

void new_game()
{
	Board *Chessboard = new Board();

	Chessboard->set_newgame();

	std::string input = "";

	do 
	{
		std::cout << "\n" << display_board(*Chessboard);
		std::cout << "\nwhite > ";
		std::cin >> input;
	} while (!(input == "abort") && !(Chessboard->is_checkmated()));

	std::cout << std::endl;
}