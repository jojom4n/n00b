#ifndef MOVEORDER_H
#define MOVEORDER_H

#include "defs.h"
#include "Position.h"
#include "search.h"
#include <vector>

std::vector<Move> ordering(std::vector<Move> const &moves, Position const &p, short const &depth, 
    Search const &search);
std::vector<Move> mvv_lva(std::vector<Move> const &moves);

#endif
