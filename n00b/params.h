#ifndef PARAMS_H
#define PARAMS_H

#define TT_BIT (8000000 / 96) // MB (8 mln bits) / 96-bit TT

constexpr unsigned long long PERFT_CACHE_SIZE = 0x2DC6C0; // perft cache size (in number of positions)
constexpr unsigned long long TT_SIZE = 5 * TT_BIT; // megabytes dedicated to TT


#endif
