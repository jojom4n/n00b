#ifndef MAKEMOVE_H
#define MAKEMOVE_H

#include "defs.h"

class Position;

void doMove(Move const& m, Position& p);
void undoMove(Move const& m, Position& p); // new undoMove

#endif