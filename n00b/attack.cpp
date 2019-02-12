#include "pch.h"
#include <iostream>
#include "defs.h"
#include "protos.h"

struct Mask Masks;

void initAttacks()
{
	// initialize the std::array arrays in Masks struct
	Masks.file.fill({});
	Masks.rank.fill({});
	Masks.rays.fill({});
	Masks.rook.fill({});
	
	lineAttacks();
	rayAttacks();
	generateRookMask(Masks);
	rookMagic();
}

void generateRookMask(struct Mask m)
{
	for (ushort square = A1; square <= H8; square++) {
		if (square <= H7) m.rays[NORTH][square] ^= C64(1) << bitscan_rvs(m.rays[NORTH][square]);
		if (square >= A2) m.rays[SOUTH][square] ^= C64(1) << bitscan_fwd(m.rays[SOUTH][square]);
		if (FILE_INDEX != FILE_H) m.rays[EAST][square] ^= C64(1) << bitscan_rvs(m.rays[EAST][square]);
		if (FILE_INDEX != FILE_A) m.rays[WEST][square] ^= C64(1) << bitscan_fwd(m.rays[WEST][square]);
		Masks.rook[square] = m.rays[NORTH][square] | m.rays[SOUTH][square] | m.rays[EAST][square] | m.rays[WEST][square];
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
}

void rayAttacks()
{
	// N attack. We don't precalculate rank 8 because there is no north from there
	for (ushort square = A1; square <= H7; square++)
		// we will shift up the rank mask by (rank index + 1) - +1 needed because rank index starts from 0
		Masks.rays[NORTH][square] = Masks.file[FILE_INDEX] << (8 * (RANK_INDEX + 1));

	// S attack. We don't precalculate rank 1 because there is no south from there
	for (ushort square = A2; square <= H8; square++)
		// we will shift towards down the rank mask by (8 - rank index)
		Masks.rays[SOUTH][square] = Masks.file[FILE_INDEX] >> (8 * (8 - RANK_INDEX));

	// E attack. We don't precalculate file H because there is no east from there
	for (ushort square = A1; square <= H8; square++)
		if (FILE_INDEX != FILE_H)
			/* Shifting appropriate rankMask n times, where n is the file index (+1 because file index starts from 0).
			Since shifting will create clipping, we AND the result of the shift with the same rankMask again, to reset clipped '1' */
			Masks.rays[EAST][square] = Masks.rank[RANK_INDEX] << (1 * (FILE_INDEX + 1)) & Masks.rank[RANK_INDEX];

	// W attack. We don't precalculate file A because there is no west from there
	for (ushort square = A1; square <= H8; square++)
		if (FILE_INDEX != FILE_A)
			/* Shifting appropriate rankMask n times, where n is (8 - file index).
			Since shifting will create clipping, we AND the result of the shift with the same rankMask again, to reset clipped '1' */
			Masks.rays[WEST][square] = Masks.rank[RANK_INDEX] >> (1 * (8 - FILE_INDEX)) & Masks.rank[RANK_INDEX];
}