#include "pch.h"
#include "attack.h"
#include "bitscan.h"
#include "overloading.h"

Mask g_Masks{};
LookupTable g_MoveTables{};

void initAttacks()
{		
	// generate rays, with and without edges (suffix -Ex)
	linesAttacks();
	raysAttacks();
	raysEx();
	SlidingMaskEx();
	
	// generate attacks
	kingMask();
	knightMask();
	rookMagic();
	bishopMagic();
	whitePawnMask();
	blackPawnMask();
}


void SlidingMaskEx()
{
	g_Masks.linesEx.fill({});
	g_Masks.diagonalsEx.fill({});
	
	for (Square square = A1; square <= H8; square++) {
		
		g_Masks.linesEx[square] = g_Masks.raysEx[NORTH][square] | g_Masks.raysEx[SOUTH][square] 
			| g_Masks.raysEx[WEST][square] | g_Masks.raysEx[EAST][square];
		
		g_Masks.diagonalsEx[square] = g_Masks.raysEx[NORTH_WEST][square] | g_Masks.raysEx[NORTH_EAST][square] 
			| g_Masks.raysEx[SOUTH_WEST][square] | g_Masks.raysEx[SOUTH_EAST][square];
	}
}


void linesAttacks()
{
	// FILES AND RANKS
	g_Masks.file.fill({});
	g_Masks.rank.fill({});

	for (ushort f = FILE_A, square = f; f <= FILE_H; f++, square = f)
		for (Rank r = RANK_1; r <= RANK_8; r++, square += 8) 
			g_Masks.file[f] |= C64(1) << square;

	for (ushort r = RANK_1, square = 0; r <= RANK_8; r++)
		for (File f = FILE_A; f <= FILE_H; f++, square += 1) 
			g_Masks.rank[r] |= C64(1) << square;

	
	// DIAGONALS
	uint64_t baseDiagonal = C64(0x8040201008040201); // diagonal from A1 to H8
	g_Masks.diagonalsEx.fill({});
	
	/* calculate and store diagonals from A1 to H1.
	We use a mask to remove all excessive bits coming from baseDiagonal << 1 */
	for (File f = FILE_A; f <= FILE_H; f++, baseDiagonal = (baseDiagonal << 1) & C64(0x80c0e0f0f8fcfe))
		g_Masks.diagonal[7 - f] = baseDiagonal;
	
	baseDiagonal = C64(0x4020100804020100); // reset baseDiagonal to diagonal from A2 to H7

	for (Rank r = RANK_2; r <= RANK_8; r++, baseDiagonal = (baseDiagonal >> 1) & C64(0x7f3f1f0f07030100))
		g_Masks.diagonal[7 + r] = baseDiagonal;
	
	uint64_t baseAntiDiagonal = C64(0x102040810204080); // antidiagonal from A8 to H1

	/* calculate and store anti-diagonals from A8 to H8.
	We use a mask to remove all excessive bits coming from baseAntiDiagonal << 1 */
	for (File f = FILE_A; f <= FILE_H; f++, baseAntiDiagonal = (baseAntiDiagonal << 1) & C64(0xfefcf8f0e0c08000))
		g_Masks.antiDiagonal[(RANK_8 + f)] = baseAntiDiagonal;

	baseAntiDiagonal = C64(0x1020408102040); // reset baseDiagonal to diagonal from A7 to H2

	for (Rank r = RANK_7; r >= RANK_1; r--, baseAntiDiagonal = (baseAntiDiagonal >> 1) & C64(0x103070f1f3f7f))
		g_Masks.antiDiagonal[r] = baseAntiDiagonal;
}


void raysAttacks()
{
	// N ray
	uint64_t baseN = C64(0x0101010101010100);
	for (Square square = A1; square <= H8; square++, baseN <<= 1)
		g_Masks.rays[NORTH][square] = baseN;

	// S ray
	uint64_t baseS = C64(0x0080808080808080);
	for (Square square = H8; square >= A1; square--, baseS >>= 1)
		g_Masks.rays[SOUTH][square] = baseS;

	// W ray
	uint64_t baseW = C64(0x7f00000000000000);
	for (Square square = H8; square >= A1; square--, baseW >>= 1)
		g_Masks.rays[WEST][square] = baseW & g_Masks.rank[RANK_INDEX];

	// E ray
	uint64_t baseE = C64(0xfe);
	for (Square square = A1; square <= H8; square++, baseE <<=1)
		g_Masks.rays[EAST][square] = baseE & g_Masks.rank[RANK_INDEX];
	
	// NW ray
	uint64_t baseDiagonalNW = C64(0x102040810204000);

	for (File f = FILE_H; f >= FILE_A; f--, baseDiagonalNW = (baseDiagonalNW >> 1) & NOT_FILE_H) {
		uint64_t copyDiagonal = baseDiagonalNW;

		for (ushort r = RANK_1; r < SQ_NUMBER; r += 8, copyDiagonal <<= 8)
			g_Masks.rays[NORTH_WEST][r + f] = copyDiagonal;
	}

	// NE ray
	uint64_t baseDiagonalNE = C64(0x8040201008040200);

	for (File f = FILE_A; f <= FILE_H; f++, baseDiagonalNE = (baseDiagonalNE << 1) & NOT_FILE_A) {
		uint64_t copyDiagonal = baseDiagonalNE;

		for (ushort r = RANK_1; r < SQ_NUMBER; r += 8, copyDiagonal <<= 8)
			g_Masks.rays[NORTH_EAST][r + f] = copyDiagonal;
	}
	
	// SW ray
	uint64_t baseDiagonalSW = C64(0x40201008040201);

	for (File f = FILE_H; f >= FILE_A; f--, baseDiagonalSW = (baseDiagonalSW >> 1) & NOT_FILE_H) {
		uint64_t copyDiagonal = baseDiagonalSW;

		for (short r = RANK_8; r >= RANK_1; r -= 1, copyDiagonal >>= 8)
			g_Masks.rays[SOUTH_WEST][(C64(8) * r) + f] = copyDiagonal;
	}
	
	// SE ray
	uint64_t baseDiagonalSE = C64(0x2040810204080);

	for (File f = FILE_A; f <= FILE_H; f++, baseDiagonalSE = (baseDiagonalSE << 1) & NOT_FILE_A) {
		uint64_t copyDiagonal = baseDiagonalSE;

		for (short r = RANK_8; r >= RANK_1; r -= 1, copyDiagonal >>= 8)
			g_Masks.rays[SOUTH_EAST][(C64(8) * r) + f] = copyDiagonal;
	}
}


void raysEx() 
{
	g_Masks.raysEx = g_Masks.rays;

	for (Square square = A1; square <= H8; square++) {
		if (square <= H7) g_Masks.raysEx[NORTH][square] ^= C64(1) << bitscan_rvs(g_Masks.raysEx[NORTH][square]);
		if (square >= A2) g_Masks.raysEx[SOUTH][square] ^= C64(1) << bitscan_fwd(g_Masks.raysEx[SOUTH][square]);
		if (!(FILE_INDEX == FILE_A)) g_Masks.raysEx[WEST][square] ^= C64(1) << bitscan_fwd(g_Masks.raysEx[WEST][square]);
		if (!(FILE_INDEX == FILE_H)) g_Masks.raysEx[EAST][square] ^= C64(1) << bitscan_rvs(g_Masks.raysEx[EAST][square]);

		if (g_Masks.raysEx[NORTH_WEST][square]) 
			g_Masks.raysEx[NORTH_WEST][square] ^= C64(1) << bitscan_rvs(g_Masks.raysEx[NORTH_WEST][square]);
		
		if (g_Masks.raysEx[NORTH_EAST][square]) 
			g_Masks.raysEx[NORTH_EAST][square] ^= C64(1) << bitscan_rvs(g_Masks.raysEx[NORTH_EAST][square]);
		
		if (g_Masks.raysEx[SOUTH_WEST][square]) 
			g_Masks.raysEx[SOUTH_WEST][square] ^= C64(1) << bitscan_fwd(g_Masks.raysEx[SOUTH_WEST][square]);
		
		if (g_Masks.raysEx[SOUTH_EAST][square]) 
			g_Masks.raysEx[SOUTH_EAST][square] ^= C64(1) << bitscan_fwd(g_Masks.raysEx[SOUTH_EAST][square]);
	}
}


void kingMask()
{
	g_MoveTables.king.fill({});

	for (Square square = A1; square <= H8; square++) {
		uint64_t kingPosition{0}, north, south, west, east, northWest, southWest, northEast, southEast;

		kingPosition |= C64(1) << square;
		north = kingPosition << 8;
		south = kingPosition >> 8;
		west = (kingPosition & NOT_FILE_A) >> 1;
		east = (kingPosition & NOT_FILE_H) << 1;
		northWest = (kingPosition & NOT_FILE_A) << 7;
		southWest = (kingPosition & NOT_FILE_A) >> 9;
		northEast = (kingPosition & NOT_FILE_H) << 9;
		southEast = (kingPosition & NOT_FILE_H) >> 7;

		g_MoveTables.king[square] = north | south | west | east | northWest | southWest
			| northEast | southEast;
	}
}


void knightMask()
{
	g_MoveTables.knight.fill({});

	for (Square square = A1; square <= H8; square++) {
		uint64_t knightPosition{ 0 }, nnw, nne, ne, se, sse, ssw, sw, nw;

		knightPosition |= C64(1) << square;
		nnw = (knightPosition & NOT_FILE_A) << 15;
		nne = (knightPosition & NOT_FILE_H) << 17;
		ne = (knightPosition & NOT_FILE_GH) << 10;
		se = (knightPosition & NOT_FILE_GH) >> 6;
		sse = (knightPosition & NOT_FILE_H) >> 15;
		ssw = (knightPosition & NOT_FILE_A) >> 17;
		sw = (knightPosition & NOT_FILE_AB) >> 10;
		nw = (knightPosition & NOT_FILE_AB) << 6;

		g_MoveTables.knight[square] = nnw | nne | ne | se | sse | ssw | sw | nw;
	}
}


const Bitboard LookupTable::rook(Square const &square, Bitboard const &blockers) const
{
#ifdef USE_BMI2
#include <immintrin.h>

	return g_MoveTables.rookMagic[square]
		[(int)(_pext_u64(blockers, g_Masks.linesEx[square]))];
#else
	return g_MoveTables.rookMagic[square]
		[((blockers & g_Masks.linesEx[square]) * MAGIC_ROOK[square]) >> SHIFT_ROOK[square]];
#endif
}


const Bitboard LookupTable::bishop(Square const &square, Bitboard const &blockers) const
{
#ifdef USE_BMI2
#include <immintrin.h>
	
	return g_MoveTables.bishopMagic[square]
		[(int)(_pext_u64(blockers, g_Masks.diagonalsEx[square]))];

#else
	
	return g_MoveTables.bishopMagic[square]
		[((blockers & g_Masks.diagonalsEx[square]) * MAGIC_BISHOP[square]) >> SHIFT_BISHOP[square]];
#endif
}

void whitePawnMask()
{
	g_MoveTables.whitePawn.fill({});

	for (Square square = A1; square <= H7; square++) {
		auto sqToBit = C64(1) << square;
		g_MoveTables.whitePawn[square] = ((sqToBit << 7) & NOT_FILE_H) | ((sqToBit << 9) & NOT_FILE_A);
	}
}


void blackPawnMask()
{
	g_MoveTables.blackPawn.fill({});

	for (Square square = A2; square <= H8; square++) {
		auto sqToBit = C64(1) << square;
		g_MoveTables.blackPawn[square] = ((sqToBit >> 7) & NOT_FILE_A) | ((sqToBit >> 9) & NOT_FILE_H);
	}
}

/* const Bitboard LookupTable::whitePawn(Bitboard const &pawn, Bitboard const &occupancy) const
{
	return (((pawn << 7) & NOT_FILE_H) | ((pawn << 9) & NOT_FILE_A)) & occupancy;
}

const Bitboard LookupTable::blackPawn(Bitboard const &pawn, Bitboard const &occupancy) const
{
	return (((pawn >> 7) & NOT_FILE_A) | ((pawn >> 9) & NOT_FILE_H)) & occupancy;
} */
