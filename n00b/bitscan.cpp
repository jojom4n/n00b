#include "pch.h"
#include "protos.h"
#include "defs.h"

//now the BitScanForward and BitScanReverse by hardware
#if defined(__GNUC__)  // GCC, Clang, ICC

inline const Square bitscan_fwd(Bitboard const &b)
{
	assert(b);
	return Square(__builtin_ctzll(b));
}

inline const Square Board::bitscan_rvs(Bitboard const &b) const
{
	assert(b);
	return Square(63 ^ __builtin_clzll(b));
}

#elif defined(_MSC_VER)  // MSVC

#ifdef _WIN64  // MSVC, WIN64

const ushort bitscan_fwd(Bitboard const &b)
{
	assert(b);
	unsigned long index;
	_BitScanForward64(&index, b);
	return (ushort)index;
}

const ushort bitscan_rvs(Bitboard const &b)
{
	assert(b);
	unsigned long index;
	_BitScanReverse64(&index, b);
	return (ushort)index;
}

#else  // MSVC, WIN32

const ushort bitscan_fwd(Bitboard const &b)
{
	assert(b);
	unsigned long index;

	if (b & 0xffffffff) {
		_BitScanForward(&index, int32_t(b));
		return ushort(index);
	}
	else {
		_BitScanForward(&index, int32_t(b >> 32));
		return ushort(index + 32);
	}
}

const ushort bitscan_rvs(Bitboard const &b)
{
	assert(b);
	unsigned long index;

	if (b >> 32) {
		_BitScanReverse(&index, int32_t(b >> 32));
		return ushort(index + 32);
	}
	else {
		_BitScanReverse(&index, int32_t(b));
		return ushort(index);
	}
}

#endif

#else  // Compiler is neither GCC nor MSVC compatible

#error "Compiler not supported."

#endif

const ushort bitscan_reset(Bitboard &b)
{
	ushort index = bitscan_fwd(b);
	b &= b - 1; // reset bit outside
	return index;
}

const ushort popcnt(Bitboard const &b) {
	Bitboard count = b;
	count = count - ((count >> 1) & k1);
	count = (count & k2) + ((count >> 2)  & k2);
	count = (count + (count >> 4)) & k4;
	count = (count * kf) >> 56;
	return (ushort)count;
}