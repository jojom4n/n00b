#ifndef FEN_H
#define FEN_H

#include <sstream>
#include <string>

class Position;

const bool fenValidate(std::stringstream& fen);
void fenParser(std::stringstream& fen, Position& board);
std::string positionToFEN(const Position& board);

#endif
