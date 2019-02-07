#include "pch.h"
#include <iostream>
#include <bitset>
#include "Board.h"
#include "defs.h"

void new_game()
{
	Board *Chessboard = new Board();

	Chessboard->set_newgame();

	//For testing purposes only
	std::cout << "\nPosizione completa\n" 
		<< std::bitset<64>(Chessboard->get_position()) << std::endl << std::endl;

	std::cout << std::bitset<64>(Chessboard->get_position()) << std::endl; //end testing

	std::string input;

	do 
	{
		std::cout << "\nwhite > ";
		std::cin >> input;
	} while (!(input == "abort") && !(Chessboard->is_checkmated()));

	std::cout << std::endl;
}