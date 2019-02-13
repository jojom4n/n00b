#include "pch.h"
#include <iostream>
#include <bitset> // REMOVE BEFORE RELEASE. FOR TESTING ONLY
#include "defs.h"
#include "protos.h"

struct Mask Masks;

void initAttacks()
{
	// initialize the std::array arrays in Masks struct
	Masks.file.fill({});
	Masks.rank.fill({});
	Masks.raysNoEdge.fill({});
	Masks.rook.fill({});
	
	// generate attacks and masks
	lineAttacks();
	raysAttacks();
	raysNoEdge();
	generateSlidingPieceMask(Masks);
	rookMagic();
	bishopMagic();
}


void generateSlidingPieceMask(struct Mask m)
{
	for (ushort square = A1; square <= H8; square++) {
		
		Masks.rook[square] = m.raysNoEdge[NORTH][square] | m.raysNoEdge[SOUTH][square] 
			| m.raysNoEdge[WEST][square] | m.raysNoEdge[EAST][square];
		
		Masks.bishop[square] = m.raysNoEdge[NORTH_WEST][square] | m.raysNoEdge[NORTH_EAST][square] 
			| m.raysNoEdge[SOUTH_WEST][square] | m.raysNoEdge[SOUTH_EAST][square];
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
	uint64_t baseDiagonal = C64(0x8040201008040201); // diagonal from A1 to H8
	
	/* calculate and store diagonals from A1 to H1.
	We use a mask to remove all excessive bits coming from baseDiagonal << 1 */
	for (ushort f = FILE_A; f <= FILE_H; f++, baseDiagonal = (baseDiagonal << 1) & C64(0x80c0e0f0f8fcfe))
		Masks.diagonal[7 - f] = baseDiagonal;
	
	baseDiagonal = C64(0x4020100804020100); // reset baseDiagonal to diagonal from A2 to H7

	for (ushort r = RANK_2; r <= RANK_8; r++, baseDiagonal = (baseDiagonal >> 1) & C64(0x7f3f1f0f07030100))
		Masks.diagonal[7 + r] = baseDiagonal;
	

	// Anti-diagonal mask
	uint64_t baseAntiDiagonal = C64(0x102040810204080); // antidiagonal from A8 to H1

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
	uint64_t baseN = C64(0x0101010101010100);
	for (ushort square = A1; square <= H8; square++, baseN <<= 1)
		Masks.rays[NORTH][square] = baseN;
	

	// S ray
	uint64_t baseS = C64(0x0080808080808080);
	for (short square = H8; square >= A1; square--, baseS >>= 1)
		Masks.rays[SOUTH][square] = baseS;


	// W ray
	uint64_t baseW = C64(0x7f00000000000000);
	for (short square = H8; square >= A1; square--, baseW >>= 1)
		Masks.rays[WEST][square] = baseW & Masks.rank[RANK_INDEX];


	// E ray
	uint64_t baseE = C64(0xfe);
	for (ushort square = A1; square <= H8; square++, baseE <<=1)
		Masks.rays[EAST][square] = baseE & Masks.rank[RANK_INDEX];
	

	// NW ray
	uint64_t baseDiagonalNW = C64(0x102040810204000);

	for (short f = FILE_H; f >= FILE_A; f--, baseDiagonalNW = (baseDiagonalNW >> 1) & NOT_FILE_H) {
		uint64_t copyDiagonal = baseDiagonalNW;

		for (ushort r = RANK_1; r < SQ_NUMBER; r += 8, copyDiagonal <<= 8)
			Masks.rays[NORTH_WEST][r + f] = copyDiagonal;
	}
	

	// NE ray
	uint64_t baseDiagonalNE = C64(0x8040201008040200);

	for (ushort f = FILE_A; f <= FILE_H; f++, baseDiagonalNE = (baseDiagonalNE << 1) & NOT_FILE_A) {
		uint64_t copyDiagonal = baseDiagonalNE;
		
		for (ushort r = RANK_1; r < SQ_NUMBER; r += 8, copyDiagonal <<= 8)
			Masks.rays[NORTH_EAST][r + f] = copyDiagonal;
	}


	// SW ray
	uint64_t baseDiagonalSW = C64(0x40201008040201);

	for (short f = FILE_H; f >= FILE_A; f--, baseDiagonalSW = (baseDiagonalSW >> 1) & NOT_FILE_H) {
		uint64_t copyDiagonal = baseDiagonalSW;

		for (short r = RANK_8; r >= RANK_1; r -= 1, copyDiagonal >>= 8)
			Masks.rays[SOUTH_WEST][(8 * r) + f] = copyDiagonal;
	}
	

	// SE ray
	uint64_t baseDiagonalSE = C64(0x2040810204080);

	for (ushort f = FILE_A; f <= FILE_H; f++, baseDiagonalSE = (baseDiagonalSE << 1) & NOT_FILE_A) {
		uint64_t copyDiagonal = baseDiagonalSE;

		for (short r = RANK_8; r >= RANK_1; r -= 1, copyDiagonal >>= 8)
			Masks.rays[SOUTH_EAST][(8 * r) + f] = copyDiagonal;
	}
}


void raysNoEdge() 
{
	// use NO_EDGE mask to extract No Edge Masks from simple rays
	for (ushort square = A1; square <= H8; square++) {
		Masks.raysNoEdge[NORTH][square] = Masks.rays[NORTH][square] & NO_EDGES;
		Masks.raysNoEdge[SOUTH][square] = Masks.rays[SOUTH][square] & NO_EDGES;
		Masks.raysNoEdge[WEST][square] = Masks.rays[WEST][square] & NO_EDGES;
		Masks.raysNoEdge[EAST][square] = Masks.rays[EAST][square] & NO_EDGES;
		Masks.raysNoEdge[NORTH_WEST][square] = Masks.rays[NORTH_WEST][square] & NO_EDGES;
		Masks.raysNoEdge[NORTH_EAST][square] = Masks.rays[NORTH_EAST][square] & NO_EDGES;
		Masks.raysNoEdge[SOUTH_WEST][square] = Masks.rays[SOUTH_WEST][square] & NO_EDGES;
		Masks.raysNoEdge[SOUTH_EAST][square] = Masks.rays[SOUTH_EAST][square] & NO_EDGES;
	}
}