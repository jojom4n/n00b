#ifndef PROTOS_H
#define PROTOS_H

#include <string>
#include "defs.h"

// commands.cpp
void read_commands();
Options resolve_input(const std::string const &input);

// game.cpp
void new_game();

#endif