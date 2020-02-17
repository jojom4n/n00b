#ifndef MOVEORDER_H
#define MOVEORDER_H

#include "defs.h"
#include <vector>

std::vector<Move> ordering(std::vector<Move> const& moves);
std::vector<Move> mvv_lva(std::vector<Move> const& moves);

#endif
