#ifndef ATTACK_H
#define ATTACK_H

#include "enums.h"
#include "magic.h"
#include <array>
#include <intrin.h>

struct Mask {
	std::array<Bitboard, FILE_NUMBER> file;
	std::array<Bitboard, RANK_NUMBER> rank;
	std::array<Bitboard, DIAG_NUMBER> diagonal;
	std::array<Bitboard, DIAG_NUMBER> antiDiagonal;
	std::array<Bitboard, SQ_NUMBER> linesEx;
	std::array<Bitboard, SQ_NUMBER> diagonalsEx;
	std::array<std::array<Bitboard, SQ_NUMBER>, RAYS_NUMBER>rays;
	std::array<std::array<Bitboard, SQ_NUMBER>, RAYS_NUMBER>raysEx;
};

struct LookupTable {
	alignas(64) std::array<Bitboard, SQ_NUMBER> king;
	alignas(64) std::array<Bitboard, SQ_NUMBER> knight;
	alignas(64) std::array<std::array<Bitboard, 1 << ROOK_INDEX_BITS>, SQ_NUMBER> rookMagic;
	alignas(64) std::array<std::array<Bitboard, 1 << BISHOP_INDEX_BITS>, SQ_NUMBER> bishopMagic;
	alignas(64) std::array<Bitboard, SQ_NUMBER> whitePawn;
	alignas(64) std::array<Bitboard, SQ_NUMBER> blackPawn;
	/* const Bitboard whitePawn(Bitboard const& pawn, Bitboard const& occupancy) const;
	const Bitboard blackPawn(Bitboard const& pawn, Bitboard const& occupancy) const; */
	const Bitboard rook(Square const& square, Bitboard const& blockers) const;
	const Bitboard bishop(Square const& square, Bitboard const& blockers) const;
};

void initAttacks();
void SlidingMaskEx();
void linesAttacks();
void raysAttacks();
void raysEx();
void kingMask();
void knightMask();
void whitePawnMask();
void blackPawnMask();

#endif
