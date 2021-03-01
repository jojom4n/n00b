#include "pch.h"
#include "magic.h"
#include "attack.h"
#include "bitscan.h"
#include "overloading.h"

// see attack.cpp (and its header file)
extern struct Mask g_Masks; 
extern struct LookupTable g_MoveTables;

void rookMagic()
{
	g_MoveTables.rookMagic.fill({});

	for (Square square = A1; square <= H8; square++) {
		std::array<Bitboard, 1 << ROOK_INDEX_BITS> blockerboard;
		blockerboard.fill({});
		Bitboard tmp_rook{};
		ushort bits = ushort(popcount(&g_Masks.linesEx[square]));

		for (ushort i = 0; i < (1 << bits); i++) {
			blockerboard[i] = gen_blockerboard(i, bits, g_Masks.linesEx[square]);
			tmp_rook = gen_r_attks(square, blockerboard[i]);
			
#ifdef USE_BMI2
#include <immintrin.h>
			uint64_t index = _pext_u64(blockerboard[i], g_Masks.linesEx[square]);
#else
			uint64_t index = ((blockerboard[i] & g_Masks.linesEx[square])
				* MAGIC_ROOK[square]) >> SHIFT_ROOK[square];
#endif
			
			g_MoveTables.rookMagic[square][ushort(index)] = tmp_rook;
		}
	}
}


void bishopMagic()
{
	g_MoveTables.bishopMagic.fill({});

	for (Square square = A1; square <= H8; square++) {
		std::array<Bitboard, 1 << BISHOP_INDEX_BITS> blockerboard;
		blockerboard.fill({});
		Bitboard tmp_bishop{};
		ushort bits = ushort(popcount(&g_Masks.diagonalsEx[square]));

		for (ushort i = 0; i < (1 << bits); i++) {
			blockerboard[i] = gen_blockerboard(i, bits, g_Masks.diagonalsEx[square]);
			tmp_bishop = gen_b_attks(square, blockerboard[i]);

#ifdef USE_BMI2
#include <immintrin.h>
			uint64_t index = _pext_u64(blockerboard[i], g_Masks.diagonalsEx[square]);
#else
			uint64_t index = ((blockerboard[i] & g_Masks.diagonalsEx[square])
				* MAGIC_BISHOP[square]) >> SHIFT_BISHOP[square];
#endif

			g_MoveTables.bishopMagic[square][ushort(index)] = tmp_bishop;
		}
	}
}


const Bitboard gen_blockerboard(ushort const &index, ushort const &bits, Bitboard b)
{
	Bitboard result{};

	for (ushort i = 0, j = 0; i < bits; i++) {
		j = bitscan_reset(b);
		if (index & (1 << i)) result |= (C64(1) << j);
	}

	return result;
}


const Bitboard gen_r_attks(Square const &square, Bitboard const &blockerboard)
{
	Bitboard result{};

	for (Rank r = Rank(RANK_INDEX + 1); r <= 7; r++) {
		result |= C64(1) << (FILE_INDEX + r * 8);
		if (blockerboard & (C64(1) << (FILE_INDEX + r * 8))) break;
	}

	for (Rank r = Rank(RANK_INDEX - 1); r >= 0; r--) {
		result |= C64(1) << (FILE_INDEX + r * 8);
		if (blockerboard & (C64(1) << (FILE_INDEX + r * 8))) break;
	}

	for (File f = File(FILE_INDEX + 1); f <= 7; f++) {
		result |= C64(1) << (f + RANK_INDEX * 8);
		if (blockerboard & (C64(1) << (f + RANK_INDEX * 8))) break;
	}

	for (File f = File(FILE_INDEX - 1); f >= 0; f--) {
		result |= C64(1) << (f + RANK_INDEX * 8);
		if (blockerboard & (C64(1) << (f + RANK_INDEX * 8))) break;
	}
	
	return result;
}


const Bitboard gen_b_attks(Square const &square, Bitboard const &blockerboard)
{
	Bitboard result{};

	for (ushort r = RANK_INDEX + 1, f = FILE_INDEX + 1;
		r <= RANK_8 && f <= FILE_H; r++, f++)
	{
		result |= C64(1) << (f + r * 8);
		if (blockerboard & (C64(1) << (f + r * 8))) break;
	}

	for (short r = RANK_INDEX + 1, f = FILE_INDEX - 1;
		r <= RANK_8 && f >= FILE_A; r++, f--)
	{
		result |= C64(1) << (f + r * 8);
		if (blockerboard & (C64(1) << (f + r * 8))) break;
	}

	for (short r = RANK_INDEX - 1, f = FILE_INDEX + 1;
		r >= RANK_1 && f <= FILE_H; r--, f++)
	{
		result |= C64(1) << (f + r * 8);
		if (blockerboard & (C64(1) << (f + r * 8))) break;
	}

	for (short r = RANK_INDEX - 1, f = FILE_INDEX - 1;
		r >= RANK_1 && f >= FILE_A; r--, f--)
	{
		result |= C64(1) << (f + r * 8);
		if (blockerboard & (C64(1) << (f + r * 8))) break;
	}

	return result;
}
