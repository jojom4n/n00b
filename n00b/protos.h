#ifndef PROTOS_H
#define PROTOS_H

#include <string>
#include "defs.h"
#include "Board.h"

// tables.cpp
void init_tables();
void create_file_mask();
void create_rank_mask();
void create_attacks_from();

// game.cpp
void new_game();

//display.cpp
const std::string display_board(Board const &board);
const char print_piece(bb_index const &coordinates);

#endif