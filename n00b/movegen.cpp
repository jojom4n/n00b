#include "pch.h"
#include "Position.h"
#include "defs.h"
#include "protos.h"

extern struct LookupTable MoveTables;
std::vector<Move> moveList{};

std::vector<Move> moveGeneration(Position const &board)
{
	moveList.clear();
	Color sideToMove = board.getTurn(); // which side are we generating moves for? 
	Check check{};
	const Bitboard occupancy = board.getPosition();
	const Bitboard ownPieces = board.getPosition(sideToMove);

	for (Piece p = KING; p <= PAWN; p++) {
		Bitboard bb = board.getPieces(sideToMove, p);
		
		while (bb) { // loop until the bitboard has a piece on it
			Square squareFrom = Square(bitscan_reset(bb)); // find the square(s) with the particular piece;
			Bitboard moves{};
			
			// get move bitboard for the piece, given its square
			switch (p) {
			case KING:
				moves = MoveTables.king[squareFrom];
				break;
			case QUEEN:
				moves = MoveTables.bishop(squareFrom, occupancy)
					| MoveTables.rook(squareFrom, occupancy);
				break;
			case ROOK:
				moves = MoveTables.rook(squareFrom, occupancy);
				break;
			case KNIGHT:
				moves = MoveTables.knight[squareFrom];
				break;
			case BISHOP:
				moves = MoveTables.bishop(squareFrom, occupancy);
				break;
			case PAWN: 
				/* For pawns, it's a bit more complicated, because they have no lookup tables.
				For readibility, better pass the params to an appropriate function */
				moves = pawnMoves(sideToMove, squareFrom, occupancy, ownPieces);		
				break;
			}

			if (!(p == PAWN))
				moves &= ~ownPieces; // exclude own pieces from moves
			
			while (moves) { // scan collected moves, determine their type and add them to list
				Square squareTo = Square(bitscan_reset(moves));
				MoveType type = setType(p, moves, board, squareFrom, squareTo);	
				check = isCheck(p, squareTo, board); // is the move a check to opponent king?
				Piece captured = board.idPiece(squareTo).piece;
				
				if (captured == KING) break; // captured can't be enemy king

				if (type == CAPTURE) {
					Move m = composeMove(squareFrom, squareTo, sideToMove, p, type, captured, 0, check);
					moveList.push_back(m);
				}
				else if (type == PROMOTION) { 
					// compose two moves: the first promoting to knight, the second to queen
					Move m = composeMove(squareFrom, squareTo, sideToMove, p, type, captured, PAWN_TO_KNIGHT, check);
					Move m2 = composeMove(squareFrom, squareTo, sideToMove, p, type, captured, PAWN_TO_QUEEN, check);
					moveList.push_back(m);
					moveList.push_back(m2);
				}
				else {
					Move m = composeMove(squareFrom, squareTo, sideToMove, p, type, captured, 0, check);
					moveList.push_back(m);
				}
			} // end while (moves)
		} // end while (bb)
	} // end for loop

	castleMoves(board, check);
	enPassant(board, check);
	return moveList;
}


const Bitboard pawnMoves(Color const &c, Square const &from, Bitboard const &occ, Bitboard const &own)
{
	Bitboard m{};

	(c == WHITE) ? m |= ((C64(1) << from) << 8) & ~occ
		: m |= ((C64(1) << from) >> 8) & ~occ;

	// Is the pawn on 2nd rank(or 7th rank, for black)? If yes, let it move even two squares ahead
	if (c == WHITE && (from / 8) == RANK_2)
		m ^= (m << 8) & ~occ;
	else if (c == BLACK && (from / 8) == RANK_7)
		m ^= (m >> 8) & ~occ;

	/* Set in an empty bitboard the bit corresponding to 'from' (i.e. where pawn is)
	and compute if it can attack from there opponent's pieces*/
	(c == WHITE) ? m |= MoveTables.whitePawn(C64(1) << from, ~own) & occ
		: m |= MoveTables.blackPawn(C64(1) << from, ~own) & occ;

	return m;
}


void castleMoves(Position const &p, Check isCheck)
{
	Move m{};
	Color c = p.getTurn();
	Bitboard occ = p.getPosition();

	if (p.getCastle(c) == QUEENSIDE) {

		if (p.idPiece(A8).piece == ROOK && p.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(A8, occ) >> E8) & C64(1))
				m = composeMove(A8, D8, c, ROOK, CASTLE_Q, NO_PIECE, 0, isCheck);
		}
		else if (p.idPiece(A1).piece == ROOK && p.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(A1, occ) >> E1) & C64(1))
				m = composeMove(A1, D1, c, ROOK, CASTLE_Q, NO_PIECE, 0, isCheck);
		}

		if (m) moveList.push_back(m);

	}

	else if (p.getCastle(c) == KINGSIDE) {

		if (p.idPiece(H8).piece == ROOK && p.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(H8, occ) >> E8) & C64(1))
				m = composeMove(H8, F8, c, ROOK, CASTLE_K, NO_PIECE, 0, isCheck);
		}
		else if (p.idPiece(H1).piece == ROOK && p.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(H1, occ) >> E1) & C64(1))
				m = composeMove(H1, F1, c, ROOK, CASTLE_K, NO_PIECE, 0, isCheck);
		}

		if (m) moveList.push_back(m);
	}

	else if (p.getCastle(c) == ALL) {

		if (p.idPiece(A8).piece == ROOK && p.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(A8, occ) >> E8) & C64(1))
				m = composeMove(A8, D8, c, ROOK, CASTLE_Q, NO_PIECE, 0, isCheck);
		}
		else if (p.idPiece(A1).piece == ROOK && p.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(A1, occ) >> E1) & C64(1))
				m = composeMove(A1, D1, c, ROOK, CASTLE_Q, NO_PIECE, 0, isCheck);
		}

		if (m) moveList.push_back(m);

		if (p.idPiece(H8).piece == ROOK && p.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(H8, occ) >> E8) & C64(1))
				m = composeMove(H8, F8, c, ROOK, CASTLE_K, NO_PIECE, 0, isCheck);
		}
		else if (p.idPiece(H1).piece == ROOK && p.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(H1, occ) >> E1) & C64(1))
				m = composeMove(H1, F1, c, ROOK, CASTLE_K, NO_PIECE, 0, isCheck);
		}

		if (m) moveList.push_back(m);
	}
}


void enPassant(Position const &p, Check const &isCheck)
{
	Color c = p.getTurn();

	if (!(p.getEnPassant() == SQ_EMPTY)) {	// is there an en-passant square in the position?
		Square const enpassant = p.getEnPassant();
		Bitboard moves{};
		Move m;

		switch (c) {
		case WHITE: // is there a white pawn attacking the en-passant square?
			if (p.idPiece(Square(enpassant - 7)).color == WHITE && p.idPiece(Square(enpassant - 7)).piece == PAWN) {
				Square squareFrom = Square(enpassant - 7);
				m = composeMove(squareFrom, enpassant, c, PAWN, EN_PASSANT, PAWN, 0, isCheck);
			}
			else if (p.idPiece(Square(enpassant - 9)).color == WHITE && p.idPiece(Square(enpassant - 9)).piece == PAWN) {
				Square squareFrom = Square(enpassant - 9);
				m = composeMove(squareFrom, enpassant, c, PAWN, EN_PASSANT, PAWN, 0, isCheck);
			}
			break;
		case BLACK: // is there a black pawn attacking the en-passant square?
			if (p.idPiece(Square(enpassant + 7)).color == BLACK && p.idPiece(Square(enpassant + 7)).piece == PAWN) {
				Square squareFrom = Square(enpassant + 7);
				m = composeMove(squareFrom, enpassant, c, PAWN, EN_PASSANT, PAWN, 0, isCheck);
			}
			else if (p.idPiece(Square(enpassant + 9)).color == BLACK && p.idPiece(Square(enpassant + 9)).piece == PAWN) {
				Square squareFrom = Square(enpassant + 9);
				m = composeMove(squareFrom, enpassant, c, PAWN, EN_PASSANT, PAWN, 0, isCheck);
			}
			break;
		} // end switch	

		moveList.push_back(m);
	} // end if
}


const Check isCheck(Piece const &piece, Square const &sq, Position const &p) {
	Square opponentKing = Square(bitscan_fwd(p.getPieces(Color(!p.getTurn()), KING)));
	Bitboard attacksToKing{};
	const Bitboard occ = p.getPosition();
	const Bitboard own = p.getPosition(p.getTurn());

	switch (piece) {
	case QUEEN:
		attacksToKing = MoveTables.bishop(sq, occ)
			| MoveTables.rook(sq, occ);
		break;
	case ROOK:
		attacksToKing = MoveTables.rook(sq, occ);
		break;
	case KNIGHT:
		attacksToKing = MoveTables.knight[sq];
		break;
	case BISHOP:
		attacksToKing = MoveTables.bishop(sq, occ);
		break;
	case PAWN:
		(p.getTurn() == WHITE) ? attacksToKing |= MoveTables.whitePawn(C64(1) << sq, occ)
			: attacksToKing |= MoveTables.blackPawn(C64(1) << sq, occ);
		break;
	}

	while (attacksToKing) {
		Square squareTo = Square(bitscan_reset(attacksToKing));
		if (ushort(squareTo) == ushort(opponentKing)) return CHECK;
	}

	return NO_CHECK;
}


const MoveType setType(Piece const &piece, Bitboard const &m, Position const &p, Square const &from, Square const &to)
{
	Bitboard occ = p.getPosition();
	MoveType type;
	
	if ((occ >> to) & 1ULL) {
		type = CAPTURE; // if destination square is occupied by enemy...
	}
	else if (piece == PAWN && (from / 8) == RANK_7 && (to / 8) == RANK_8)
		type = PROMOTION; // is the move a pawn promotion?
	else
		type = QUIET;

	return type;
}


const Move composeMove(Square const &squareFrom, Square const &squareTo,
	Color const &color, ushort const &piece, MoveType const &type, Piece const &captured,
	bool const &promoteTo, Check const &check)
{
	
	/* combine squareFrom, squareTo and type into a binary number
	We use a 32-bit number (Move = uint32_t, see defs.h), composed in the following way:

	00000000   	000000		 000000		  0		 000	    000			     000			   0		0
	 unused    SquareFrom	SquareTo	Color	Piece	Type of move	Piece captured,    Promotion  Check?
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

	if (check == 1)
		move = (move << 1) | 1;
	else
		move = (move << 1) | 0;
	
	// update the list of moves with the new move
	return move;
}

