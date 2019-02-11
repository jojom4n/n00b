#include "pch.h"
#include <iostream>
#include <bitset>
#include "defs.h"
#include "protos.h"

Bitboard file_mask[FILE_NUMBER] {};
Bitboard rank_mask[RANK_NUMBER] {};
Bitboard rook_mask[SQ_NUMBER] {};
Bitboard north_attack[SQ_NUMBER] {}, south_attack[SQ_NUMBER] {},
	east_attack[SQ_NUMBER] {}, west_attack[SQ_NUMBER] {};

void init_tables()
{
	create_file_mask();
	create_rank_mask();
	create_attacks_from();
	create_rook_mask();
	init_rook();
}

void create_rook_mask()
{
	// we don't want to modify real file and rank masks, so we copy them in new Bitboards
	Bitboard temp_N[SQ_NUMBER]{}, temp_S[SQ_NUMBER]{}, temp_E[SQ_NUMBER]{}, temp_W[SQ_NUMBER]{};

	std::copy(&north_attack[0], &north_attack[SQ_NUMBER], temp_N);
	std::copy(&south_attack[0], &south_attack[SQ_NUMBER], temp_S);
	std::copy(&east_attack[0], &east_attack[SQ_NUMBER], temp_E);
	std::copy(&west_attack[0], &west_attack[SQ_NUMBER], temp_W);
	
	for (ushort square = A1; square <= H8; square++) {
		if (square <= H7) temp_N[square] ^= C64(1) << bitscan_rvs(temp_N[square]);
		if (square >= A2) temp_S[square] ^= C64(1) << bitscan_fwd(temp_S[square]);
		if (FILE_INDEX != FILE_H) temp_E[square] ^= C64(1) << bitscan_rvs(temp_E[square]);
		if (FILE_INDEX != FILE_A) temp_W[square] ^= C64(1) << bitscan_fwd(temp_W[square]);
		rook_mask[square] = temp_N[square] | temp_S[square] | temp_E[square] | temp_W[square];
	}
}

void create_file_mask()
{
	ushort square = 0;

	for (ushort f = FILE_A; f <= FILE_H; f++) {
		square = f; // Every time we move to a new file, we reset the square index to the value of "f"	
		for (ushort r = RANK_1; r <= RANK_8; r++) {
			file_mask[f] |= C64(1) << square;
			square += 8;
		}
	}
}

void create_rank_mask()
{
	ushort square = 0;

	for (ushort r = RANK_1; r <= RANK_8; r++) {
		for (ushort f = FILE_A; f <= FILE_H; f++) {
			rank_mask[r] |= C64(1) << square;
			square += 1;
		}
	}
}

void create_attacks_from()
{
	// N attack. We don't precalculate rank 8 because there is no north from there
	for (ushort square = A1; square <= H7; square++)
		// we will shift up the rank mask by (rank index + 1) - +1 needed because rank index starts from 0
		north_attack[square] = file_mask[FILE_INDEX] << (8 * (RANK_INDEX + 1));

	// S attack. We don't precalculate rank 1 because there is no south from there
	for (ushort square = A2; square <= H8; square++) 
		// we will shift towards down the rank mask by (8 - rank index)
		south_attack[square] = file_mask[FILE_INDEX] >> (8 * (8 - RANK_INDEX));
	
	// E attack. We don't precalculate file H because there is no east from there
	for (ushort square = A1; square <= H8; square++) 
		if (FILE_INDEX != FILE_H) 
			/* Shifting appropriate rank_mask n times, where n is the file index (+1 because file index starts from 0).
			Since shifting will create clipping, we AND the result of the shift with the same rank_mask again, to reset clipped '1' */
			east_attack[square] = rank_mask[RANK_INDEX] << (1 * (FILE_INDEX + 1)) & rank_mask[RANK_INDEX];
	
	// W attack. We don't precalculate file A because there is no west from there
	for (ushort square = A1; square <= H8; square++)
		if (FILE_INDEX != FILE_A)
			/* Shifting appropriate rank_mask n times, where n is (8 - file index).
			Since shifting will create clipping, we AND the result of the shift with the same rank_mask again, to reset clipped '1' */
			west_attack[square] = rank_mask[RANK_INDEX] >> (1 * (8 - FILE_INDEX)) & rank_mask[RANK_INDEX];
}