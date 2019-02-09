#include "pch.h"
#include "defs.h"
#include <iostream>
#include <bitset>

void init_masks() {
	Bitboard file_mask[FILE_NUMBER], rank_mask[RANK_NUMBER];
	ushort square_index = 0;

	// Initialize the bitboards in file_mask, one bitboard for each file
	for (ushort f = FILE_A; f <= FILE_H; f++)
		file_mask[f] = C64(0);

	// Initialize the bitboards in rank_mask, one bitboard for each rank
	for (ushort r = RANK_1; r <= RANK_8; r++)
		rank_mask[r] = C64(0);

	// Fill each bitboard in file mask, setting only '1' in the respective column
	for (ushort f = FILE_A; f <= FILE_H; f++) {  // Start from file A to file H
		square_index = f; // Every time we move to a new file, we reset the square index to the value of "f"
			
		for (ushort r = RANK_1; r <= RANK_8; r++) { // For every file, we fill the '1' in the bitboard rank by rank
			file_mask[f] |= C64(1) << square_index;
			square_index += 8;
		}

	}

	square_index = 0;

	for (ushort r = RANK_1; r <= RANK_8; r++) {  // Start from file A to file H
		
		for (ushort f = FILE_A; f <= FILE_H; f++) { // For every rank, we fill the '1' in the bitboard file by file
			rank_mask[r] |= C64(1) << square_index;
			square_index += 1;
		}

	}

	std::bitset<64>number(rank_mask[RANK_2]);
	std::cout << number << std::endl;
}