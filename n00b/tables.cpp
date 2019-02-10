#include "pch.h"
#include <iostream>
#include <bitset>
#include "defs.h"
#include "protos.h"

Bitboard file_mask[FILE_NUMBER] = { C64(0) };
Bitboard rank_mask[RANK_NUMBER] = { C64(0) };
Bitboard north_attack[SQ_NUMBER] = { C64(0) }, south_attack[SQ_NUMBER] = { C64(0) },
	east_attack[SQ_NUMBER] = { C64(0) }, west_attack[SQ_NUMBER] = { C64(0) };

void init_tables()
{
	create_file_mask();
	create_rank_mask();
	create_attacks_from();
}

void create_file_mask()
{
	ushort square_index = 0;

	for (ushort f = FILE_A; f <= FILE_H; f++) {
		square_index = f; // Every time we move to a new file, we reset the square index to the value of "f"	
		for (ushort r = RANK_1; r <= RANK_8; r++) {
			file_mask[f] |= C64(1) << square_index;
			square_index += 8;
		}
	}
}

void create_rank_mask()
{
	ushort square_index = 0;

	for (ushort r = RANK_1; r <= RANK_8; r++) {
		for (ushort f = FILE_A; f <= FILE_H; f++) {
			rank_mask[r] |= C64(1) << square_index;
			square_index += 1;
		}
	}
}

void create_attacks_from()
{
	// N attack. We don't precalculate rank 8 because there is no north from there
	for (ushort square_index = A1; square_index <= H7; square_index++)
		// we will shift up the rank mask by (rank index + 1) - +1 needed because rank index starts from 0
		north_attack[square_index] = file_mask[FILE_INDEX] << (8 * (RANK_INDEX + 1));

	// S attack. We don't precalculate rank 1 because there is no south from there
	for (ushort square_index = A2; square_index <= H8; square_index++) 
		// we will shift towards down the rank mask by (8 - rank index)
		south_attack[square_index] = file_mask[FILE_INDEX] >> (8 * (8 - RANK_INDEX));
	
	// E attack. We don't precalculate file H because there is no east from there
	for (ushort square_index = A1; square_index <= H8; square_index++) 
		if (FILE_INDEX != FILE_H) 
			/* Shifting appropriate rank_mask n times, where n is the file index (+1 because file index starts from 0).
			Since shifting will create clipping, we AND the result of the shift with the same rank_mask again, to reset clipped '1' */
			east_attack[square_index] = rank_mask[RANK_INDEX] << (1 * (FILE_INDEX + 1)) & rank_mask[RANK_INDEX];
	
	// W attack. We don't precalculate file A because there is no west from there
	for (ushort square_index = A1; square_index <= H8; square_index++)
		if (FILE_INDEX != FILE_A)
			/* Shifting appropriate rank_mask n times, where n is (8 - file index).
			Since shifting will create clipping, we AND the result of the shift with the same rank_mask again, to reset clipped '1' */
			west_attack[square_index] = rank_mask[RANK_INDEX] >> (1 * (8 - FILE_INDEX)) & rank_mask[RANK_INDEX];
	
	std::bitset<64>x(west_attack[D6]);
	std::cout << x << std::endl;
}