#include "pch.h"
#include <list>
#include "Position.h"
#include "defs.h"
#include "protos.h"

extern struct LookupTable MoveTables;
std::list<Move> moveList;


void moveGeneration(Position const &board) 
{
	moveList.clear(); // let us clear the movelist. We want a new movelist for every generation
	Color sideToMove = board.getTurn(); // which side are we generating moves for? 
	const Bitboard occupancy = board.getPosition();
	const Bitboard ownPieces = board.getPosition(sideToMove);

	// ***** PSEUDO-LEGAL MOVES FOR ALL PIECE *****
	for (ushort piece = KING; piece <= PAWN; piece++)
 {
		Bitboard bb = board.getPieces(sideToMove, Piece(piece));
		MoveType type{}; // is a capture or a quiet move?
		
		while (bb) { // loop until this specific bitboard has a piece of its kind on it
			Square squareFrom = Square(bitscan_reset(bb)); // find the square(s) with the particular piece;

			Bitboard attacks{};
			
			// get attack bitboard for the piece, given its square
			switch (piece) {
			case KING:
				attacks = MoveTables.king[squareFrom];
				break;
			case QUEEN:
				attacks = MoveTables.bishop(squareFrom, occupancy)
					| MoveTables.rook(squareFrom, occupancy);
				break;
			case ROOK:
				attacks = MoveTables.rook(squareFrom, occupancy);
				break;
			case KNIGHT:
				attacks = MoveTables.knight[squareFrom];
				break;
			case BISHOP:
				attacks = MoveTables.bishop(squareFrom, occupancy);
				break;
			case PAWN: {
				// For pawns, it's a bit more elaborated. First of all, we calculate a simple advance move
				(sideToMove == WHITE) ? attacks |= ((C64(1) << squareFrom) << 8) & ~occupancy
					: attacks |= ((C64(1) << squareFrom) >> 8) & ~occupancy;

				/* Is the pawn on 2nd rank(or 7th rank, for black)?
				If yes, let it move even two squares ahead. Note: en-passant, if applicable, will not
				be set of course in Position instance, because: 1) we are manipulating a const instance;
				2) we are only looking at available (pseudo-legal) moves, we are not MAKING them on the
				board, and there will be no en-passant until the move will be made.
				Therefore, en-passant will be managed by eval & search routines. Move generation routine,
				though, will set a special bit for en-passant when adding a possible en-passant capture to
				the list (look below, updateList routine) */
				if (sideToMove == WHITE && (squareFrom / 8) == RANK_2)
					attacks ^= (attacks << 8) & ~occupancy;
				else if (sideToMove == BLACK && (squareFrom / 8) == RANK_7)
					attacks ^= (attacks >> 8) & ~occupancy;

				/* Now, we set in an empty bitboard the bit corresponding to squareFrom (i.e. where pawn is)
				and compute if it can attack from there */
				(sideToMove == WHITE) ? attacks |= MoveTables.whitePawn(C64(1) << squareFrom, occupancy)
					: attacks |= MoveTables.blackPawn(C64(1) << squareFrom, occupancy);

				break; } // end case PAWNS
			} // end switch (piece)

			attacks &= ~ownPieces; // exclude own pieces from attacks

			while (attacks) { // scan the attack bitboard. For each attack, create the move and update the list
				Square squareTo = Square(bitscan_reset(attacks));
				Piece captured{NO_PIECE};

				/* For all pieces we determine move type: if squareTo is occupied by enemy piece, then
				it's a capture. For pawns we will have to take care of en-passant outside of this whole loop */
				if ((occupancy >> squareTo) & 1ULL) {
					type = CAPTURE;
					captured = board.idPiece(squareTo).piece;
				} // end if
				else if (piece == PAWN // is the move a pawn promotion?
					&& (squareFrom / 8) == RANK_7 && (squareTo / 8) == RANK_8) { 
					type = PROMOTION;
				} // end else if
				else 
					type = QUIET;

				// ***** PAWN PROMOTIONS *****
				if (type == PROMOTION) { /* if it's a promotion, we update twice: once 
											with pawn promoting to knight, once with 
											same pawn promoting to queen */
					updateMoveList(squareFrom, squareTo, sideToMove, piece, type, captured, PAWN_TO_KNIGHT);
					updateMoveList(squareFrom, squareTo, sideToMove, piece, type, captured, PAWN_TO_QUEEN);
				} // end if
				else 
					updateMoveList(squareFrom, squareTo, sideToMove, piece, type, captured, 0);
			} // end while (attacks)

		} // end while (bb)

	} // end for loop

	
	// ***** CASTLE MOVES, IF AVAILABLE *****
	if (board.getCastle(sideToMove) == QUEENSIDE) {
		if (board.idPiece(A8).piece == ROOK && board.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(A8, occupancy) >> E8) & C64(1))
				updateMoveList(A8, D8, sideToMove, ROOK, CASTLE, NO_PIECE, 0);
		}
		else if (board.idPiece(A1).piece == ROOK && board.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(A1, occupancy) >> E1) & C64(1))
				updateMoveList(A1, D1, sideToMove, ROOK, CASTLE, NO_PIECE, 0);
		}
	}
	
	else if (board.getCastle(sideToMove) == KINGSIDE) {
		if (board.idPiece(H8).piece == ROOK && board.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(H8, occupancy) >> E8) & C64(1))
				updateMoveList(H8, F8, sideToMove, ROOK, CASTLE, NO_PIECE, 0);
		}
		else if (board.idPiece(H1).piece == ROOK && board.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(H1, occupancy) >> E1) & C64(1))
				updateMoveList(H1, F1, sideToMove, ROOK, CASTLE, NO_PIECE, 0);
		}
	}

	else if (board.getCastle(sideToMove) == ALL) {
		if (board.idPiece(A8).piece == ROOK && board.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(A8, occupancy) >> E8) & C64(1))
				updateMoveList(A8, D8, sideToMove, ROOK, CASTLE, NO_PIECE, 0);
		}
		else if (board.idPiece(A1).piece == ROOK && board.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(A1, occupancy) >> E1) & C64(1))
				updateMoveList(A1, D1, sideToMove, ROOK, CASTLE, NO_PIECE, 0);
		}

		if (board.idPiece(H8).piece == ROOK && board.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(H8, occupancy) >> E8) & C64(1))
				updateMoveList(H8, F8, sideToMove, ROOK, CASTLE, NO_PIECE, 0);
		}
		else if (board.idPiece(H1).piece == ROOK && board.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(H1, occupancy) >> E1) & C64(1))
				updateMoveList(H1, F1, sideToMove, ROOK, CASTLE, NO_PIECE, 0);
		}
	}


	// ***** EN-PASSANT MOVES, IF AVAILABLE *****
	if (board.getEnPassant() != SQ_EMPTY) {	// is there an en-passant square in the position?
		Square const enpassant = board.getEnPassant();
		Bitboard attacks{};
		
		switch (sideToMove) {
		case WHITE: // is there a white pawn attacking the en-passant square?
			if (board.idPiece(Square(enpassant - 7)).color == WHITE && board.idPiece(Square(enpassant - 7)).piece == PAWN) {
				Square squareFrom = Square(enpassant - 7);
				updateMoveList(squareFrom, enpassant, sideToMove, PAWN, EN_PASSANT, PAWN, 0);
			}
			else if (board.idPiece(Square(enpassant - 9)).color == WHITE && board.idPiece(Square(enpassant - 9)).piece == PAWN) {
				Square squareFrom = Square(enpassant - 9);
				updateMoveList(squareFrom, enpassant, sideToMove, PAWN, EN_PASSANT, PAWN, 0);
			}
			break;
		case BLACK: // is there a black pawn attacking the en-passant square?
			if (board.idPiece(Square(enpassant + 7)).color == BLACK && board.idPiece(Square(enpassant + 7)).piece == PAWN) {
				Square squareFrom = Square(enpassant + 7);
				updateMoveList(squareFrom, enpassant, sideToMove, PAWN, EN_PASSANT, PAWN, 0);
			}
			else if (board.idPiece(Square(enpassant + 9)).color == BLACK && board.idPiece(Square(enpassant + 9)).piece == PAWN) {
				Square squareFrom = Square(enpassant + 9);
				updateMoveList(squareFrom, enpassant, sideToMove, PAWN, EN_PASSANT, PAWN, 0);
			}
			break;
		} // end switch		

	} // end main 'if'
}


void updateMoveList(Square const &squareFrom, Square const &squareTo,
	Color const &color, ushort const &piece, MoveType const &type, Piece const &captured,
	bool const &promoteTo)
{
	
	/* combine squareFrom, squareTo and type into a binary number
	We use a 32-bit number (Move = uint32_t, see defs.h), composed in the following way:

	000000000   	000000		 000000		  0		 000	    000			     000			   0
	 unused       SquareFrom	SquareTo	Color	Piece	Type of move	Piece captured,    Promotion
																				if any			 to...
	
	*/

	Move move{};
	move = (move << 6) | squareFrom;
	move = (move << 6) | squareTo;
	move = (move << 1) | color;
	move = (move << 3) | piece;
	move = (move << 3) | type;
	move = (move << 3) | captured;
	
	if (promoteTo == 1)
		move = (move << 1) | 1;
	else
		move = (move << 1) | 0;
	
	// update the list of moves with the new move
	moveList.push_back(move);
}
