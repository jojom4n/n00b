#ifndef PROTOS_H
#define PROTOS_H

#include <string>
#include "defs.h"
#include "Board.h"

// commands.cpp
void read_commands();
Option resolve_input(std::string const &input);

// game.cpp
void new_game();

//display.cpp
const std::string display(Board const &board);
const char print_piece(bb_coordinates const &coordinates);

#endif