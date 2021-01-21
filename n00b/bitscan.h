#ifndef BITSCAN_H
#define BITSCAN_H

#include "defs.h"
#include "libpopcnt.h"

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

//constexpr uint64_t  c1 = UINT64_C(0x5555555555555555);
//constexpr uint64_t  c2 = UINT64_C(0x3333333333333333);
//constexpr uint64_t  c4 = UINT64_C(0x0F0F0F0F0F0F0F0F);
//
//const ushort popcount(Bitboard const& b);

const ushort bitscan_reset(Bitboard& b, bool reverse = 0);

#endif

