#include "pch.h"
#include "defs.h"
#include "protos.h"

const Bitboard rook_magic[SQ_NUMBER] =
{
	C64(0x0080001020400080), C64(0x0040001000200040), C64(0x0080081000200080), C64(0x0080040800100080),
	C64(0x0080020400080080), C64(0x0080010200040080), C64(0x0080008001000200), C64(0x0080002040800100),
	C64(0x0000800020400080), C64(0x0000400020005000), C64(0x0000801000200080), C64(0x0000800800100080),
	C64(0x0000800400080080), C64(0x0000800200040080), C64(0x0000800100020080), C64(0x0000800040800100),
	C64(0x0000208000400080), C64(0x0000404000201000), C64(0x0000808010002000), C64(0x0000808008001000),
	C64(0x0000808004000800), C64(0x0000808002000400), C64(0x0000010100020004), C64(0x0000020000408104),
	C64(0x0000208080004000), C64(0x0000200040005000), C64(0x0000100080200080), C64(0x0000080080100080),
	C64(0x0000040080080080), C64(0x0000020080040080), C64(0x0000010080800200), C64(0x0000800080004100),
	C64(0x0000204000800080), C64(0x0000200040401000), C64(0x0000100080802000), C64(0x0000080080801000),
	C64(0x0000040080800800), C64(0x0000020080800400), C64(0x0000020001010004), C64(0x0000800040800100),
	C64(0x0000204000808000), C64(0x0000200040008080), C64(0x0000100020008080), C64(0x0000080010008080),
	C64(0x0000040008008080), C64(0x0000020004008080), C64(0x0000010002008080), C64(0x0000004081020004),
	C64(0x0000204000800080), C64(0x0000200040008080), C64(0x0000100020008080), C64(0x0000080010008080),
	C64(0x0000040008008080), C64(0x0000020004008080), C64(0x0000800100020080), C64(0x0000800041000080),
	C64(0x00FFFCDDFCED714A), C64(0x007FFCDDFCED714A), C64(0x003FFFCDFFD88096), C64(0x0000040810002101),
	C64(0x0001000204080011), C64(0x0001000204000801), C64(0x0001000082000401), C64(0x0001FFFAABFAD1A2)
};

Bitboard rook_table[SQ_NUMBER][1 << ROOK_INDEX_BITS]{};

const Bitboard rook_attack (Square const &square, Bitboard blockers)
{
	return rook_table[square][((blockers&rook_mask[square]) * rook_magic[square]) >> (SQ_NUMBER - ROOK_INDEX_BITS)];
}

void create_rook_magic()
{	

}

const Bitboard gen_blockerboard(int index, Bitboard const &blockermask)
{
	/* Start with a blockerboard identical to the mask. */
	Bitboard blockerboard = blockermask;

	/* Loop through the blockermask to find the indices of all set bits. */
	ushort bitindex = 0;
	for (ushort i = 0; i < SQ_NUMBER; i++) {
		/* Check if the i'th bit is set in the mask (and thus a potential blocker). */
		if (blockermask & (1ULL << i)) {
			/* Clear the i'th bit in the blockerboard if it's clear in the index at bitindex. */
			if (!(index & (1 << bitindex))) {
				blockerboard &= ~(1ULL << i); //Clear the bit.
			}
			/* Increment the bit index in the 0-4096 index, so each bit in index will correspond
			 * to each set bit in blockermask. */
			bitindex++;
		}
	}
	return blockerboard;
}