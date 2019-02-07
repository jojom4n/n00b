#ifndef PROTOS_H
#define PROTOS_H

#include <string>
#include "defs.h"

// commands.cpp
void read_commands();
Option resolve_input(std::string const &input);

// game.cpp
void new_game();

#endif