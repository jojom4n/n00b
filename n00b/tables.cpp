#include "pch.h"
#include "defs.h"
#include <iostream>
#include <bitset>

void init_masks() {
	Bitboard file_mask[FILE_NUMBER], rank_mask[RANK_NUMBER];
	ushort square_index = 0;

	for (ushort f = FILE_A; f <= FILE_H; f++) // init the bitboards
		file_mask[f] = C64(0);

	for (ushort r = RANK_1; r <= RANK_8; r++) // init the bitboards
		rank_mask[r] = C64(0);

	// Create file mask bitboards
	for (ushort f = FILE_A; f <= FILE_H; f++) {
		square_index = f; // Every time we move to a new file, we reset the square index to the value of "f"	
		for (ushort r = RANK_1; r <= RANK_8; r++) {
			file_mask[f] |= C64(1) << square_index;
			square_index += 8;
		}
	}

	square_index = 0; // reset the square index; we'll use it again for rank bitboards
	
	// Create rank mask bitboards
	for (ushort r = RANK_1; r <= RANK_8; r++) {
		for (ushort f = FILE_A; f <= FILE_H; f++) {
			rank_mask[r] |= C64(1) << square_index;
			square_index += 1;
		}
	}

	std::bitset<64>number(rank_mask[RANK_2]);
	std::cout << number << std::endl;
}