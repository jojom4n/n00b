#ifndef FEN_H
#define FEN_H

#include <sstream>

class Position;

const bool fenValidate(std::stringstream& fen);
void fenParser(std::stringstream& fen, Position& board);

#endif
