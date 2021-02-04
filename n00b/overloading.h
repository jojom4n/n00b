#ifndef OVERLOADING_H
#define OVERLOADING_H

#include "enums.h"

Square operator++(Square& s, int);
Square operator--(Square& s, int);
File operator++(File& f, int);
File operator--(File& f, int);
Rank operator++(Rank& r, int);
Rank operator--(Rank& r, int);
Piece operator++(Piece& p, int);
Piece operator--(Piece &p, int);
Color operator++(Color& c, int);

#endif
