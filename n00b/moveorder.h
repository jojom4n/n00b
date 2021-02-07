#ifndef MOVEORDER_H
#define MOVEORDER_H

#include "defs.h"
#include "Position.h"
#include <vector>

std::vector<Move> ordering(std::vector<Move> &moves, Position const& p);
std::vector<Move> mvv_lva(std::vector<Move> const& moves);

#endif
