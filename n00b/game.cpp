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

	while (!(input == "quit") && !(input == "q") && !(Chessboard->is_checkmate())) 
	{
		std::cout << "\n" << display_board(*Chessboard);

		if (Chessboard->has_move() == white)
			std::cout << "\nwhite";
		else
			std::cout << "\nblack";

		read_commands();
	}
}