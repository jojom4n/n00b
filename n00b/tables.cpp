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
	//create N attack. We don't precalculate rank 8 because there is no north from there
	for (ushort square_index = A1; square_index <= H7; square_index++) { 
		// north_attack[square_index] |= C64(1) << square_index;
		north_attack[square_index] = file_mask[square_index % 8] << (8 * (square_index >> 3));
	}

	//create S attack. We don't precalculate rank 1 because there is no south from there
	for (ushort square_index = A2; square_index <= H8; square_index++) {
		ushort file_no = square_index >> 3; 
		short shift = file_no + 1; // we add 1 to have the real rank (indexes start from 0, in fact)
		shift = 8 - shift; // we will shift towards down the rank row by (8 - shift)
		// south_attack[square_index] |= C64(1) << square_index;
		south_attack[square_index] ^= file_mask[square_index % 8] >> (8 * shift);
	}
	
	//create E attack. We don't precalculate file H because there is no east from there
	for (ushort square_index = A1; square_index <= H8; square_index++) {
		if ((square_index % 8) != FILE_H) {
			ushort rank_no = square_index >> 3; // which rank we are upon?
			short shift = (square_index % 8) + 1; // take file, add 1 (because files are indexed from 0)
												  // and prepare to shift appropriate rank_mask n times, where n is
												  // the file number
			
			/* Bitboard temp_mask = rank_mask[rank_no]; // make a temp copy of rank mask
			
			for (; shift > 0; shift--)
				temp_mask = temp_mask << 1; // shift the copy of rank mask

			temp_mask &= rank_mask[rank_no]; // shifting to right creates clipping, so let's remove it
			east_attack[square_index] ^= temp_mask; */

			/* So, we now shift appropriate rank mask; since shifting will create clipping, we
			AND the result of the shift with the same rank_mask again, to delete clipped '1' */
			east_attack[square_index] = (rank_mask[rank_no] << (1 * shift) & rank_mask[rank_no]);


		}
	}
	
	std::bitset<64>x(north_attack[A1]);
	std::cout << x << std::endl;
}