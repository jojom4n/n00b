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

	for (;;)
	{
		std::string input;
		if (Chessboard->has_move() == white)
			std::cout << "\nwhite";
		else
			std::cout << "\nblack";

		std::cin >> input;
	}
}