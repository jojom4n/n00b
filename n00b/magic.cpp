#include "pch.h"
#include "magic.h"
#include "attack.h"
#include "bitscan.h"
#include "overloading.h"

const Bitboard MAGIC_ROOK[64] = {
	0x0080004000608010L, 0x2240100040012002L, 0x008008a000841000L,
	0x0100204900500004L, 0x020008200200100cL, 0x40800c0080020003L,
	0x0080018002000100L, 0x4200042040820d04L, 0x10208008a8400480L,
	0x4064402010024000L, 0x2181002000c10212L, 0x5101000850002100L,
	0x0010800400080081L, 0x0012000200300815L, 0x060200080e002401L,
	0x4282000420944201L, 0x1040208000400091L, 0x0010004040002008L,
	0x0082020020804011L, 0x0005420010220208L, 0x8010510018010004L,
	0x05050100088a1400L, 0x0009008080020001L, 0x2001060000408c01L,
	0x0060400280008024L, 0x9810401180200382L, 0x0200201200420080L,
	0x0280300100210048L, 0x0000080080800400L, 0x0002010200081004L,
	0x8089000900040200L, 0x0040008200340047L, 0x0400884010800061L,
	0xc202401000402000L, 0x0800401301002004L, 0x4c43502042000a00L,
	0x0004a80082800400L, 0xd804040080800200L, 0x060200080e002401L,
	0x0203216082000104L, 0x0000804000308000L, 0x004008100020a000L,
	0x1001208042020012L, 0x0400220088420010L, 0x8010510018010004L,
	0x8009000214010048L, 0x6445006200130004L, 0x000a008402460003L,
	0x0080044014200240L, 0x0040012182411500L, 0x0003102001430100L,
	0x4c43502042000a00L, 0x1008000400288080L, 0x0806003008040200L,
	0x4200020801304400L, 0x8100640912804a00L, 0x300300a043168001L,
	0x0106610218400081L, 0x008200c008108022L, 0x0201041861017001L,
	0x00020010200884e2L, 0x0205000e18440001L, 0x202008104a08810cL,
	0x800a208440230402L
};

const Bitboard MAGIC_BISHOP[64] = {
	0x010a0a1023020080L, 0x0050100083024000L, 0x8826083200800802L,
	0x0102408100002400L, 0x0414242008000000L, 0x0414242008000000L,
	0x0804230108200880L, 0x0088840101012000L, 0x0400420202041100L,
	0x0400420202041100L, 0x1100300082084211L, 0x0000124081000000L,
	0x0405040308000411L, 0x01000110089c1008L, 0x0030108805101224L,
	0x0010808041101000L, 0x2410002102020800L, 0x0010202004098180L,
	0x1104000808001010L, 0x274802008a044000L, 0x1400884400a00000L,
	0x0082000048260804L, 0x4004840500882043L, 0x0081001040680440L,
	0x4282180040080888L, 0x0044200002080108L, 0x2404c80a04002400L,
	0x2020808028020002L, 0x0129010050304000L, 0x0008020108430092L,
	0x005600450c884800L, 0x005600450c884800L, 0x001004501c200301L,
	0xa408025880100100L, 0x1042080300060a00L, 0x4100a00801110050L,
	0x11240100c40c0040L, 0x24a0281141188040L, 0x08100c4081030880L,
	0x020c310201002088L, 0x006401884600c280L, 0x1204028210809888L,
	0x8000a01402005002L, 0x041d8a021a000400L, 0x041d8a021a000400L,
	0x000201a102004102L, 0x0408010842041282L, 0x000201a102004102L,
	0x0804230108200880L, 0x0804230108200880L, 0x8001010402090010L,
	0x0008000042020080L, 0x4200012002440000L, 0x80084010228880a0L,
	0x4244049014052040L, 0x0050100083024000L, 0x0088840101012000L,
	0x0010808041101000L, 0x1090c00110511001L, 0x2124000208420208L,
	0x0800102118030400L, 0x0010202120024080L, 0x00024a4208221410L,
	0x010a0a1023020080L
};

const ushort SHIFT_ROOK[64] = {
	52, 53, 53, 53, 53, 53, 53, 52,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	52, 53, 53, 53, 53, 53, 53, 52,
};

const ushort SHIFT_BISHOP[64] = {
	58, 59, 59, 59, 59, 59, 59, 58,
	59, 59, 59, 59, 59, 59, 59, 59,
	59, 59, 57, 57, 57, 57, 59, 59,
	59, 59, 57, 55, 55, 57, 59, 59,
	59, 59, 57, 55, 55, 57, 59, 59,
	59, 59, 57, 57, 57, 57, 59, 59,
	59, 59, 59, 59, 59, 59, 59, 59,
	58, 59, 59, 59, 59, 59, 59, 58
};

// see attack.cpp (and its header file)
extern struct Mask Masks; 
extern struct LookupTable MoveTables;

void rookMagic()
{
	MoveTables.rookMagic.fill({});

	for (Square square = A1; square <= H8; square++) {
		std::array<Bitboard, 1 << ROOK_INDEX_BITS> blockerboard;
		blockerboard.fill({});
		Bitboard tmp_rook{};
		ushort bits = popcount(Masks.linesEx[square]);

		for (ushort i = 0; i < (1 << bits); i++) {
			blockerboard[i] = gen_blockerboard(i, bits, Masks.linesEx[square]);
			tmp_rook = gen_r_attks(square, blockerboard[i]);
			
			uint64_t index = ((blockerboard[i] & Masks.linesEx[square]) 
				* MAGIC_ROOK[square]) >> SHIFT_ROOK[square];
			
			MoveTables.rookMagic[square][ushort(index)] = tmp_rook;
		}
	}
}


void bishopMagic()
{
	MoveTables.bishopMagic.fill({});

	for (Square square = A1; square <= H8; square++) {
		std::array<Bitboard, 1 << BISHOP_INDEX_BITS> blockerboard;
		blockerboard.fill({});
		Bitboard tmp_bishop{};
		ushort bits = popcount(Masks.diagonalsEx[square]);

		for (ushort i = 0; i < (1 << bits); i++) {
			blockerboard[i] = gen_blockerboard(i, bits, Masks.diagonalsEx[square]);
			tmp_bishop = gen_b_attks(square, blockerboard[i]);

			uint64_t index = ((blockerboard[i] & Masks.diagonalsEx[square]) 
				* MAGIC_BISHOP[square]) >> SHIFT_BISHOP[square];

			MoveTables.bishopMagic[square][ushort(index)] = tmp_bishop;
		}
	}
}


const Bitboard gen_blockerboard(ushort const &index, ushort const &bits, Bitboard b)
{
	Bitboard result{};

	for (ushort i = 0, j = 0; i < bits; i++) {
		j = bitscan_reset(b);
		if (index & (1 << i)) result |= (C64(1) << j);
	}

	return result;
}


const Bitboard gen_r_attks(Square const &square, Bitboard const &blockerboard)
{
	Bitboard result{};

	for (Rank r = Rank(RANK_INDEX + 1); r <= 7; r++) {
		result |= C64(1) << (FILE_INDEX + r * 8);
		if (blockerboard & (C64(1) << (FILE_INDEX + r * 8))) break;
	}

	for (Rank r = Rank(RANK_INDEX - 1); r >= 0; r--) {
		result |= C64(1) << (FILE_INDEX + r * 8);
		if (blockerboard & (C64(1) << (FILE_INDEX + r * 8))) break;
	}

	for (File f = File(FILE_INDEX + 1); f <= 7; f++) {
		result |= C64(1) << (f + RANK_INDEX * 8);
		if (blockerboard & (C64(1) << (f + RANK_INDEX * 8))) break;
	}

	for (File f = File(FILE_INDEX - 1); f >= 0; f--) {
		result |= C64(1) << (f + RANK_INDEX * 8);
		if (blockerboard & (C64(1) << (f + RANK_INDEX * 8))) break;
	}
	
	return result;
}


const Bitboard gen_b_attks(Square const &square, Bitboard const &blockerboard)
{
	Bitboard result{};

	for (ushort r = RANK_INDEX + 1, f = FILE_INDEX + 1;
		r <= RANK_8 && f <= FILE_H; r++, f++)
	{
		result |= C64(1) << (f + r * 8);
		if (blockerboard & (C64(1) << (f + r * 8))) break;
	}

	for (short r = RANK_INDEX + 1, f = FILE_INDEX - 1;
		r <= RANK_8 && f >= FILE_A; r++, f--)
	{
		result |= C64(1) << (f + r * 8);
		if (blockerboard & (C64(1) << (f + r * 8))) break;
	}

	for (short r = RANK_INDEX - 1, f = FILE_INDEX + 1;
		r >= RANK_1 && f <= FILE_H; r--, f++)
	{
		result |= C64(1) << (f + r * 8);
		if (blockerboard & (C64(1) << (f + r * 8))) break;
	}

	for (short r = RANK_INDEX - 1, f = FILE_INDEX - 1;
		r >= RANK_1 && f >= FILE_A; r--, f--)
	{
		result |= C64(1) << (f + r * 8);
		if (blockerboard & (C64(1) << (f + r * 8))) break;
	}

	return result;
}
