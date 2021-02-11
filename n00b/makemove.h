#ifndef MAKEMOVE_H
#define MAKEMOVE_H

#include "defs.h"

class Position;

bool doMove(Move const &m, Position &p);
bool doQuickMove(Move const &m, Position &p);
void undoMove(Move const &m, Position &p);

#endif