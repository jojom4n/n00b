#ifndef PARAMS_H
#define PARAMS_H

#define TT_BIT (8000000 / 128) // MB (8 mln bits) / 128-bit TT

constexpr unsigned long long PERFT_CACHE_SIZE = 1000000; // perft cache size (in number of positions)
constexpr unsigned long long TT_SIZE = 64 * TT_BIT; // megabytes dedicated to TT


#define MAX_PLY 128

#endif
