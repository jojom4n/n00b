#include "pch.h"
#include <iostream>
#include <bitset>
#include "Board.h"
#include "Player.h"
#include "defs.h"

void new_game()
{
	Board *Chessboard = new Board();
	Player *P_white = new Player(), *P_black = new Player();
	P_black->has_move(false); // start of the game: only white has move

	/* TODO: when FEN will be implemented, we should call FEN parser function
	rather than using the following raw, inefficient code */

	Chessboard->set_newgame();
	std::cout << "---- T E S T ----" << std::endl;
	std::cout << "\nPedoni Bianco\n" 
		<< std::bitset<64>(Chessboard->get_piece_table(white, pawns)) << std::endl;
	std::cout << "\nCavalli Bianco\n" 
		<< std::bitset<64>(Chessboard->get_piece_table(white, knights)) << std::endl;
	std::cout << "\nAlfieri Bianco\n" 
		<< std::bitset<64>(Chessboard->get_piece_table(white, bishops)) << std::endl;
	std::cout << "\nTorri Bianco\n" 
		<< std::bitset<64>(Chessboard->get_piece_table(white, rooks)) << std::endl;
	std::cout << "\nRe Bianco\n" 
		<< std::bitset<64>(Chessboard->get_piece_table(white, king)) << std::endl;
	std::cout << "\nDonna Bianco\n"
		<< std::bitset<64>(Chessboard->get_piece_table(white, queen)) << std::endl;
	std::cout << "\nPosizione completa\n" 
		<< std::bitset<64>(Chessboard->get_position()) << std::endl << std::endl;
}