#ifndef PROTOS_H
#define PROTOS_H

#include <string>
#include "defs.h"
#include "Board.h"

// game.cpp
void new_game();

//display.cpp
const std::string display_board(Board const &board);
const char print_piece(bb_coordinates const &coordinates);

#endif