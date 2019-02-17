#include "pch.h"
#include <list>
#include "Position.h"
#include "defs.h"
#include "protos.h"

extern struct LookupTable AttackTables;
std::list<Move> moveList;

/* TODO: MoveGeneration special function when Position returns king under check, so that we only
generate evasion moves */

/* TODO: CASTLE and PROMOTION */

void moveGeneration(Position const &board) {
	moveList.clear(); // let us clear the movelist. We want a new movelist for every generation
	Color sideToMove = board.getTurn(); // which side are we generating moves for? 
	const Bitboard occupancy = board.getPosition();
	const Bitboard ownPieces = board.getPosition(sideToMove);

	// Pseudo-legal move generation for all pieces
	for (ushort p = KING; p <= PAWNS; p++)
 {
		Bitboard bb = board.getPieces(sideToMove, Piece(p));
		MoveType type{}; // is a capture or a quiet move?
		
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
			case PAWNS: {
				// For pawns, it's a bit more elaborated. First of all, we calculate a simple advance move
				(sideToMove == WHITE) ? attacks |= ((C64(1) << squareFrom) << 8) & ~occupancy
					: attacks |= ((C64(1) << squareFrom) >> 8) & ~occupancy;

				/* Is the pawn on 2nd rank(or 7th rank, for black)?
				If yes, let it move even two squares ahead. Note: en-passant, if applicable, will not
				be set of course in Position instance, because: 1) we are manipulating a const instance;
				2) we are only looking at available (pseudo-legal) moves, we are not MAKING them on the
				board, and there will be no en-passant until the move will be made.
				Therefore, en-passant will be managed by search & eval routines. Move generation routine,
				though, will set a special bit for en-passant when adding a possible en-passant capture to
				the list (look below, updateList routine) */
				if (sideToMove == WHITE && (squareFrom / 8) == RANK_2)
					attacks ^= (attacks << 8) & ~occupancy;
				else if (sideToMove == BLACK && (squareFrom / 8) == RANK_7)
					attacks ^= (attacks >> 8) & ~occupancy;

				/* Now, we set in an empty bitboard the bit corresponding to squareFrom (i.e. where pawn is)
				and compute if it can attack from there */
				(sideToMove == WHITE) ? attacks |= AttackTables.whitePawn(C64(1) << squareFrom, occupancy)
					: attacks |= AttackTables.blackPawn(C64(1) << squareFrom, occupancy);

				break; }
			default:
				break;
			}

			attacks &= ~ownPieces; // exclude own pieces from attacks

			while (attacks) { // scan the attack bitboard. For each attack, create the move and update the list
				Square squareTo = Square(bitscan_reset(attacks));

				/* For all pieces but pawns we determine move type: if squareTo is occupied by enemy piece,
				then it's a capture. For pawns we will have to take care of en-passant outside of this whole loop */
				((occupancy >> squareTo) & 1ULL) ? type = CAPTURE : type = QUIET;
				updateMoveList(squareFrom, squareTo, type);
			}
		}

	}

	// Last, add to the list eventual en-passant moves
	if (board.getEnPassant() != SQ_EMPTY) {	// is there an en-passant square in the position?
		Square const enpassant = board.getEnPassant();
		Bitboard attacks{};

		switch (sideToMove) {
		case WHITE: // is there a white pawn attacking the en-passant square?
			if (board.idPiece(Square(enpassant - 7)).x == WHITE && board.idPiece(Square(enpassant - 7)).y == PAWNS) {
				Square squareFrom = Square(enpassant - 7);
				updateMoveList(squareFrom, enpassant, EN_PASSANT);
			}
			else if (board.idPiece(Square(enpassant - 9)).x == WHITE && board.idPiece(Square(enpassant - 9)).y == PAWNS) {
				Square squareFrom = Square(enpassant - 9);
				updateMoveList(squareFrom, enpassant, EN_PASSANT);
			}
			break;
		case BLACK: // is there a black pawn attacking the en-passant square?
			if (board.idPiece(Square(enpassant + 7)).x == BLACK && board.idPiece(Square(enpassant + 7)).y == PAWNS) {
				Square squareFrom = Square(enpassant + 7);
				updateMoveList(squareFrom, enpassant, EN_PASSANT);
			}
			else if (board.idPiece(Square(enpassant + 9)).x == BLACK && board.idPiece(Square(enpassant + 9)).y == PAWNS) {
				Square squareFrom = Square(enpassant + 9);
				updateMoveList(squareFrom, enpassant, EN_PASSANT);
			}
			break;
		default:
			break;
		}			
	}
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
