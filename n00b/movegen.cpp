#include "pch.h"
#include <list>
#include "Position.h"
#include "defs.h"
#include "protos.h"

extern struct LookupTable AttackTables;
std::list<Move> moveList;

/* TODO: MoveGeneration special function when Position returns king under check, so that we only
generate evasion moves */

void moveGeneration(Position const &board) {
	moveList.clear(); // let us clear the movelist. We want a new movelist for every generation
	Color sideToMove = board.getTurn(); // which side are we generating moves for? 
	const Bitboard occupancy = board.getPosition();
	const Bitboard ownPieces = board.getPosition(sideToMove);

	// Pseudo-legal move generation for all pieces but pawns
	for (ushort p = KING; p <= BISHOPS; p++)
 {
		Bitboard bb = board.getPieces(sideToMove, Piece(p));
		
		while (bb) { // loop until this specific bitboard has a piece of its kind on it
			Square squareFrom = Square(bitscan_reset(bb)); // find the square(s) with the particular piece;

			Bitboard attacks{};
			// get attack bitboard for the piece, given its square
			switch (p) {
			case KING:
				attacks = AttackTables.king[squareFrom];
				break;
			case QUEEN:
				attacks = AttackTables.bishop(squareFrom, occupancy)
					| AttackTables.rook(squareFrom, occupancy);
				break;
			case ROOKS:
				attacks = AttackTables.rook(squareFrom, occupancy);
				break;
			case KNIGHTS:
				attacks = AttackTables.knight[squareFrom];
				break;
			case BISHOPS:
				attacks = AttackTables.bishop(squareFrom, occupancy);
				break;
			default:
				break;
			}

			attacks &= ~ownPieces; // exclude own pieces from attacks

			while (attacks) { // scan the attack bitboard. For each attack, create the move and update the list
				Square squareTo = Square(bitscan_reset(attacks));
				MoveType type{}; // is a capture or a quiet move?

				// determine move type
				((occupancy >> squareTo) & 1ULL) ? type = CAPTURE : type = QUIET;
				updateMoveList(squareFrom, squareTo, type);
			}
		}

	}

	/* For pawns, it's a bit more elaborated. First of all, we calculate captures, if any */
	Bitboard attacks{};
	(sideToMove == WHITE) ? attacks = AttackTables.whitePawn(board.getPieces(WHITE, PAWNS), occupancy) 
		: attacks = AttackTables.blackPawn(board.getPieces(BLACK,PAWNS), occupancy);
	
	
	
	if (board.getEnPassant() != SQ_EMPTY) attacks &= board.getEnPassant();
}


void updateMoveList(Square const &squareFrom, Square const &squareTo,
	MoveType const &type, PromotionTo const &promoteTo) {
	
	/* combine squareFrom, squareTo and type into a binary number
	We use a 16-bit number (Move = uint16_t), composed in the following way (left to right):
	bits 0-6: squareFrom
	bits 7-12: squareTo
	bits 13-15: type of move (see enum MoveType in defs.h)
	bit 16: promotion (if any) to PAWN_TO_QUEEN (value 0) or PAWN_TO_KNIGHT (value 1) */
	Move move = squareFrom;
	move = (move << 6) | squareTo;
	move = (move << 3) | type;
	move = (move << 1) | promoteTo;
	
	// update the list of moves with the new move
	moveList.push_back(move);
}
