#include "pch.h"
#include "protos.h"

//now the BitScanForward and BitScanReverse by hardware
#if defined(__GNUC__)  // GCC, Clang, ICC

const Square bitscan_fwd(Bitboard const &b)
{
	assert(b);
	return Square(__builtin_ctzll(b));
}


const Square Board::bitscan_rvs(Bitboard const &b) const
{
	assert(b);
	return Square(63 ^ __builtin_clzll(b));
}

#elif defined(_MSC_VER)  // MSVC

#ifdef _WIN64  // MSVC, WIN64

const Square bitscan_fwd(Bitboard const &b)
{
	assert(b);
	unsigned long index;
	_BitScanForward64(&index, b);
	return (Square)index;
}


const Square bitscan_rvs(Bitboard const &b)
{
	assert(b);
	unsigned long index;
	_BitScanReverse64(&index, b);
	return (Square)index;
}

#else  // MSVC, WIN32

const Square bitscan_fwd(Bitboard const &b)
{
	assert(b);
	unsigned long index;

	if (b & 0xffffffff) {
		_BitScanForward(&index, int32_t(b));
		return Square(index);
	}
	else {
		_BitScanForward(&index, int32_t(b >> 32));
		return Square(index + 32);
	}
}


const Square bitscan_rvs(Bitboard const &b)
{
	assert(b);
	unsigned long index;

	if (b >> 32) {
		_BitScanReverse(&index, int32_t(b >> 32));
		return Square(index + 32);
	}
	else {
		_BitScanReverse(&index, int32_t(b));
		return Square(index);
	}
}

#endif

#else  // Compiler is neither GCC nor MSVC compatible

#error "Compiler not supported."

#endif


const Square bitscan_reset(Bitboard &b, bool reverse)
{
	Square index{};

	if (reverse == true) {
		index = bitscan_rvs(b);
		b &= ~(C64(1) << index);
	}
	else {
		index = bitscan_fwd(b);
		b &= b - 1; // reset bit outside
	}

	return index;
}


const ushort popcount(Bitboard const &b) {
	Bitboard count = b;
	count = count - ((count >> 1) & k1);
	count = (count & k2) + ((count >> 2)  & k2);
	count = (count + (count >> 4)) & k4;
	count = (count * kf) >> 56;
	return (ushort)count;
}