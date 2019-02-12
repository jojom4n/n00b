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
	
	lineAttacks();
	rayAttacksNoEdge();
	generateRookMask(Masks);
	rookMagic();
}

void generateRookMask(struct Mask m)
{
	for (ushort square = A1; square <= H8; square++) {
		if (square <= H7) m.raysNoEdge[NORTH][square] ^= C64(1) << bitscan_rvs(m.raysNoEdge[NORTH][square]);
		if (square >= A2) m.raysNoEdge[SOUTH][square] ^= C64(1) << bitscan_fwd(m.raysNoEdge[SOUTH][square]);
		if (FILE_INDEX != FILE_H) m.raysNoEdge[EAST][square] ^= C64(1) << bitscan_rvs(m.raysNoEdge[EAST][square]);
		if (FILE_INDEX != FILE_A) m.raysNoEdge[WEST][square] ^= C64(1) << bitscan_fwd(m.raysNoEdge[WEST][square]);
		Masks.rook[square] = m.raysNoEdge[NORTH][square] | m.raysNoEdge[SOUTH][square] | m.raysNoEdge[EAST][square] | m.raysNoEdge[WEST][square];
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

void rayAttacksNoEdge() // no edge
{
	// N attack. We don't precalculate rank 8 because there is no north from there
	for (ushort square = A1; square <= H7; square++)
		// we will shift up the rank mask by (rank index + 1) - +1 needed because rank index starts from 0
		Masks.raysNoEdge[NORTH][square] = Masks.file[FILE_INDEX] << (8 * (RANK_INDEX + 1));

	// S attack. We don't precalculate rank 1 because there is no south from there
	for (ushort square = A2; square <= H8; square++)
		// we will shift towards down the rank mask by (8 - rank index)
		Masks.raysNoEdge[SOUTH][square] = Masks.file[FILE_INDEX] >> (8 * (8 - RANK_INDEX));

	// E attack. We don't precalculate file H because there is no east from there
	for (ushort square = A1; square <= H8; square++)
		if (FILE_INDEX != FILE_H)
			/* Shifting appropriate rankMask n times, where n is the file index (+1 because file index starts from 0).
			Since shifting will create clipping, we AND the result of the shift with the same rankMask again, to reset clipped '1' */
			Masks.raysNoEdge[EAST][square] = Masks.rank[RANK_INDEX] << (1 * (FILE_INDEX + 1)) & Masks.rank[RANK_INDEX];

	// W attack. We don't precalculate file A because there is no west from there
	for (ushort square = A1; square <= H8; square++)
		if (FILE_INDEX != FILE_A)
			/* Shifting appropriate rankMask n times, where n is (8 - file index).
			Since shifting will create clipping, we AND the result of the shift with the same rankMask again, to reset clipped '1' */
			Masks.raysNoEdge[WEST][square] = Masks.rank[RANK_INDEX] >> (1 * (8 - FILE_INDEX)) & Masks.rank[RANK_INDEX];
}