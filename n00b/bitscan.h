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

// for popcount() function
const Bitboard k1 = C64(0x5555555555555555); /*  -1/3   */
const Bitboard k2 = C64(0x3333333333333333); /*  -1/5   */
const Bitboard k4 = C64(0x0f0f0f0f0f0f0f0f); /*  -1/17  */
const Bitboard kf = C64(0x0101010101010101); /*  -1/255 */

const ushort popcount(Bitboard const& b);
const ushort bitscan_reset(Bitboard& b, bool reverse = 0);

#endif

