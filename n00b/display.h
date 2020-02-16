#ifndef DISPLAY_H
#define DISPLAY_H

#include "defs.h"
#include "enums.h"
#include <map>
#include <vector>
#include <string>

class Position;

void displayBoard(Position const& board);
void displayMoveList(Position const& board, std::vector<Move> const& m);
std::string displayMove(Position const& board, Move const& m);
const char printPiece(PieceID const& ID);

#endif
