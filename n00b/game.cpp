#include "pch.h"
#include <iostream>
#include <bitset>
#include <vector>
#include "Board.h"
#include "defs.h"
#include "protos.h"

void new_game()
{	
	Board *Chessboard = new Board();
	Chessboard->set_newgame();

	std::cout << std::endl << display_board(*Chessboard) << std::endl;

	std::cout << "\nNumber of white pawns: " << Chessboard->popcount(white, pawns);
	std::cout << "\nNumber of black pawns: " << Chessboard->popcount(black, pawns);
	std::cout << "\n\nNumber of white rooks: " << Chessboard->popcount(white, rooks);
	std::cout << "\nNumber of black rooks: " << Chessboard->popcount(black, rooks);
	std::cout << "\n\nNumber of white bishops: " << Chessboard->popcount(white, bishops);
	std::cout << "\nNumber of black bishops: " << Chessboard->popcount(black, bishops);
	std::cout << "\n\nNumber of white knights: " << Chessboard->popcount(white, knights);
	std::cout << "\nNumber of black knights: " << Chessboard->popcount(black, knights);
	std::cout << "\n\nNumber of white kings: " << Chessboard->popcount(white, king);
	std::cout << "\nNumber of black kings: " << Chessboard->popcount(black, king);
	std::cout << "\n\nNumber of white queens: " << Chessboard->popcount(white, queen);
	std::cout << "\nNumber of black queens: " << Chessboard->popcount(black, queen);
	std::cout << "\n\nNumber of white pieces on board: " << Chessboard->popcount(white);
	std::cout << "\nNumber of black pieces on board: " << Chessboard->popcount(black);
	std::cout << "\n\nTotal number of pieces on board: " << Chessboard->popcount();

	std::cout << "\n\nWhite King is on square: ";
	for (auto &squares : Chessboard->get_square(white, king))
		std::cout << squares_to_string[squares] << " ";
	
	std::cout << "\n\nWhite pawns are on square: ";
	for (auto &squares : Chessboard->get_square(white, pawns))
		std::cout << squares_to_string[squares] << " ";

	std::cout << "\n\nBlack rooks are on square: ";
	for (auto &squares : Chessboard->get_square(black, rooks))
		std::cout << squares_to_string[squares] << " ";

	std::cout << "\n\nBlack bishops are on square: ";
	for (auto &squares : Chessboard->get_square(black, bishops))
		std::cout << squares_to_string[squares] << " ";

	std::cout << "\n\nWhite knights are on square: ";
	for (auto &squares : Chessboard->get_square(white, knights))
		std::cout << squares_to_string[squares] << " ";
	
	Bitboard occupancy = 0x83200460a00;
	
	std::bitset<64>x(rook_attack(E3, occupancy));
	std::cout << "\n\nMagic bitboard for Rook on E3: " << x << std::endl;

	std::cout << std::endl;

	for (;;)
	{
		std::string input;
		if (Chessboard->has_move() == white)
			std::cout << "\nwhite>> ";
		else
			std::cout << "\nblack>> ";

		std::cin >> input;
	}
}