#include "pch.h"
#include "bitscan.h"
#include <assert.h>

//now the BitScanForward and BitScanReverse by hardware
#if defined(__GNUC__)  // GCC, Clang, ICC

const ushort bitscan_fwd(Bitboard const &b)
{
	assert(b);
	return ushort(__builtin_ctzll(b));
}


const ushort bitscan_rvs(Bitboard const &b)
{
	assert(b);
	return ushort(63 ^ __builtin_clzll(b));
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


const ushort bitscan_reset(Bitboard &b, const bool reverse)
{
	ushort index{};

	if (reverse) {
		index = bitscan_rvs(b);
		b &= ~(C64(1) << index);
	}
	else {
		index = bitscan_fwd(b);
		b &= b - 1; // reset bit outside
	}

	return index;
}

const ushort popcount(const Bitboard* bb)
{ 
#if defined (_MSC_VER)
	return __popcnt64(*bb);
#elif defined(__GNUC__)
	return __builtin_popcountll(*bb);
#endif

}
