#ifndef GAME_H
#define GAME_H

#include <sstream>

class Position;

void newGame();
void readCommand(std::stringstream& inputStream, Position& board);

#endif
