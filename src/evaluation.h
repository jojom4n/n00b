#ifndef EVALUATION_H
#define EVALUATION_H

#include "defs.h"
#include "enums.h"
#include <map>

class Position;

extern const std::map<Piece, ushort> g_pieceValue;

const short lazyEval(Position const& p);
const short evMaterial(Position const& p);
const short evPSQT(Position const& p);

#endif