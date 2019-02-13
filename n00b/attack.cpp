#include "pch.h"
#include <iostream>
#include "defs.h"
#include "protos.h"

struct Mask Masks;
struct AttackTable Attacks;

void initAttacks()
{
	// initialize the std::array arrays in Masks struct
	Masks.file.fill({});
	Masks.rank.fill({});
	Masks.raysEx.fill({});
	Masks.linesEx.fill({});
	Masks.diagonalsEx.fill({});
	
	Attacks.rookMagic.fill({});
	Attacks.bishopMagic.fill({});
	Attacks.king.fill({});
	
	// generate rays, with and without edges (suffix -Ex)
	lineAttacks();
	raysAttacks();
	raysEx();
	SlidingMaskEx(Masks);
	
	// generate attacks
	kingMask();
	rookMagic();
	bishopMagic();
}


void SlidingMaskEx(struct Mask m)
{
	for (ushort square = A1; square <= H8; square++) {
		
		Masks.linesEx[square] = m.raysEx[NORTH][square] | m.raysEx[SOUTH][square] 
			| m.raysEx[WEST][square] | m.raysEx[EAST][square];
		
		Masks.diagonalsEx[square] = m.raysEx[NORTH_WEST][square] | m.raysEx[NORTH_EAST][square] 
			| m.raysEx[SOUTH_WEST][square] | m.raysEx[SOUTH_EAST][square];
	}
}


void lineAttacks()
{
	// File mask
	for (ushort f = FILE_A, square = f; f <= FILE_H; f++, square = f) {
		for (ushort r = RANK_1; r <= RANK_8; r++) {
			Masks.file[f] |= C64(1) << square;
			square += 8;
		}
	}

	// Rank mask
	for (ushort r = RANK_1, square = 0; r <= RANK_8; r++) {
		for (ushort f = FILE_A; f <= FILE_H; f++) {
			Masks.rank[r] |= C64(1) << square;
			square += 1;
		}
	}


	// Diagonal mask
	U64 baseDiagonal = C64(0x8040201008040201); // diagonal from A1 to H8
	
	/* calculate and store diagonals from A1 to H1.
	We use a mask to remove all excessive bits coming from baseDiagonal << 1 */
	for (ushort f = FILE_A; f <= FILE_H; f++, baseDiagonal = (baseDiagonal << 1) & C64(0x80c0e0f0f8fcfe))
		Masks.diagonal[7 - f] = baseDiagonal;
	
	baseDiagonal = C64(0x4020100804020100); // reset baseDiagonal to diagonal from A2 to H7

	for (ushort r = RANK_2; r <= RANK_8; r++, baseDiagonal = (baseDiagonal >> 1) & C64(0x7f3f1f0f07030100))
		Masks.diagonal[7 + r] = baseDiagonal;
	

	// Anti-diagonal mask
	U64 baseAntiDiagonal = C64(0x102040810204080); // antidiagonal from A8 to H1

	/* calculate and store anti-diagonals from A8 to H8.
	We use a mask to remove all excessive bits coming from baseAntiDiagonal << 1 */
	for (ushort f = FILE_A; f <= FILE_H; f++, baseAntiDiagonal = (baseAntiDiagonal << 1) & C64(0xfefcf8f0e0c08000))
		Masks.antiDiagonal[(RANK_8 + f)] = baseAntiDiagonal;

	baseAntiDiagonal = C64(0x1020408102040); // reset baseDiagonal to diagonal from A7 to H2

	for (short r = RANK_7; r >= RANK_1; r--, baseAntiDiagonal = (baseAntiDiagonal >> 1) & C64(0x103070f1f3f7f))
		Masks.antiDiagonal[r] = baseAntiDiagonal;
}


void raysAttacks()
{
	// N ray
	U64 baseN = C64(0x0101010101010100);
	for (ushort square = A1; square <= H8; square++, baseN <<= 1)
		Masks.rays[NORTH][square] = baseN;
	

	// S ray
	U64 baseS = C64(0x0080808080808080);
	for (short square = H8; square >= A1; square--, baseS >>= 1)
		Masks.rays[SOUTH][square] = baseS;


	// W ray
	U64 baseW = C64(0x7f00000000000000);
	for (short square = H8; square >= A1; square--, baseW >>= 1)
		Masks.rays[WEST][square] = baseW & Masks.rank[RANK_INDEX];


	// E ray
	U64 baseE = C64(0xfe);
	for (ushort square = A1; square <= H8; square++, baseE <<=1)
		Masks.rays[EAST][square] = baseE & Masks.rank[RANK_INDEX];
	

	// NW ray
	U64 baseDiagonalNW = C64(0x102040810204000);

	for (short f = FILE_H; f >= FILE_A; f--, baseDiagonalNW = (baseDiagonalNW >> 1) & NOT_FILE_H) {
		U64 copyDiagonal = baseDiagonalNW;

		for (ushort r = RANK_1; r < SQ_NUMBER; r += 8, copyDiagonal <<= 8)
			Masks.rays[NORTH_WEST][r + f] = copyDiagonal;
	}
	

	// NE ray
	U64 baseDiagonalNE = C64(0x8040201008040200);

	for (ushort f = FILE_A; f <= FILE_H; f++, baseDiagonalNE = (baseDiagonalNE << 1) & NOT_FILE_A) {
		U64 copyDiagonal = baseDiagonalNE;
		
		for (ushort r = RANK_1; r < SQ_NUMBER; r += 8, copyDiagonal <<= 8)
			Masks.rays[NORTH_EAST][r + f] = copyDiagonal;
	}


	// SW ray
	U64 baseDiagonalSW = C64(0x40201008040201);

	for (short f = FILE_H; f >= FILE_A; f--, baseDiagonalSW = (baseDiagonalSW >> 1) & NOT_FILE_H) {
		U64 copyDiagonal = baseDiagonalSW;

		for (short r = RANK_8; r >= RANK_1; r -= 1, copyDiagonal >>= 8)
			Masks.rays[SOUTH_WEST][(8 * r) + f] = copyDiagonal;
	}
	

	// SE ray
	U64 baseDiagonalSE = C64(0x2040810204080);

	for (ushort f = FILE_A; f <= FILE_H; f++, baseDiagonalSE = (baseDiagonalSE << 1) & NOT_FILE_A) {
		U64 copyDiagonal = baseDiagonalSE;

		for (short r = RANK_8; r >= RANK_1; r -= 1, copyDiagonal >>= 8)
			Masks.rays[SOUTH_EAST][(8 * r) + f] = copyDiagonal;
	}
}


void raysEx() 
{
	// use NO_EDGE mask to extract No Edge Masks from simple rays
	for (ushort square = A1; square <= H8; square++) {
		Masks.raysEx[NORTH][square] = Masks.rays[NORTH][square] & NO_EDGES;
		Masks.raysEx[SOUTH][square] = Masks.rays[SOUTH][square] & NO_EDGES;
		Masks.raysEx[WEST][square] = Masks.rays[WEST][square] & NO_EDGES;
		Masks.raysEx[EAST][square] = Masks.rays[EAST][square] & NO_EDGES;
		Masks.raysEx[NORTH_WEST][square] = Masks.rays[NORTH_WEST][square] & NO_EDGES;
		Masks.raysEx[NORTH_EAST][square] = Masks.rays[NORTH_EAST][square] & NO_EDGES;
		Masks.raysEx[SOUTH_WEST][square] = Masks.rays[SOUTH_WEST][square] & NO_EDGES;
		Masks.raysEx[SOUTH_EAST][square] = Masks.rays[SOUTH_EAST][square] & NO_EDGES;
	}
}


void kingMask()
{
	for (ushort square = A1; square <= H8; square++) {
		U64 kingPosition{0}, north, south, west, east, northWest, southWest, northEast, southEast;

		kingPosition |= C64(1) << square;
		north = kingPosition << 8;
		south = kingPosition >> 8;
		west = (kingPosition & NOT_FILE_A) >> 1;
		east = (kingPosition & NOT_FILE_H) << 1;
		northWest = (kingPosition & NOT_FILE_A) << 7;
		southWest = (kingPosition & NOT_FILE_A) >> 9;
		northEast = (kingPosition & NOT_FILE_H) << 9;
		southEast = (kingPosition & NOT_FILE_H) >> 7;

		Attacks.king[square] = north | south | west | east | northWest | southWest
			| northEast | southEast;
	}
}


const Bitboard getRookAttacks(Square const &square, Bitboard const &blockers)
{
	return Attacks.rookMagic[square]
		[((blockers & Masks.linesEx[square]) * MAGIC_ROOK[square]) >> SHIFT_ROOK[square]];
}


const Bitboard getBishopAttacks(Square const & square, Bitboard const & blockers)
{
	return Attacks.bishopMagic[square]
		[((blockers & Masks.diagonalsEx[square]) * MAGIC_BISHOP[square]) >> SHIFT_BISHOP[square]];
}