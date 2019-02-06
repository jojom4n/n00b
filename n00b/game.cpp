#include "pch.h"
#include <iostream>
#include <bitset>
#include "Board.h"
#include "defs.h"

void new_game()
{
	Board *chessboard = new Board();
	std::cout << std::bitset<64>(chessboard->get_piece(white, pawns)) << std::endl;
	std::cout << std::bitset<64>(chessboard->get_position(white)) << std::endl;
}