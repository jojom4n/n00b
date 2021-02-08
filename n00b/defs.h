#ifndef DEFS_H
#define DEFS_H

#include <chrono>

#define C64(constantuint64_t) constantuint64_t##ULL
#define FILE_INDEX (square % 8)
#define RANK_INDEX (square / 8)
#define USEFUL_SQUARES 10

using Clock = std::chrono::high_resolution_clock;
using ushort = unsigned short;
using Bitboard = uint64_t;
using Move = uint32_t;

#endif