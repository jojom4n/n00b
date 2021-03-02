#ifndef NNUEEVAL_H
#define NNUEEVAL_H

#include "Position.h"

void init_NNUE(const char* filename);
int evalFEN_NNUE(const char* fen);
int eval_NNUE(Position const& p);
void fill_NNUE(Position const& p, int* nnue_pieces, int* nnue_squares);

#endif
