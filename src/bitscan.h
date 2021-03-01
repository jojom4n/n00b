#ifndef BITSCAN_H
#define BITSCAN_H

#include "defs.h"

#if defined(__GNUC__)  // GCC, Clang, ICC
const ushort bitscan_fwd(Bitboard const& b);
const ushort bitscan_rvs(Bitboard const& b);
#elif defined(_MSC_VER)  // MSVC
#ifdef _WIN64  // MSVC, WIN64
const ushort bitscan_fwd(Bitboard const& b);
const ushort bitscan_rvs(Bitboard const& b);
#else  // MSVC, WIN32
const ushort bitscan_fwd(Bitboard const& b);
const ushort bitscan_rvs(Bitboard const& b);
#endif
#else  // Compiler is neither GCC nor MSVC compatible
#error "Compiler not supported."
#endif

const ushort bitscan_reset(Bitboard& b, const bool reverse = 0);
const ushort popcount(const Bitboard* bb);

#endif

