#include "pch.h"
#include <list>
#include "Position.h"
#include "defs.h"
#include "protos.h"

extern struct LookupTable MoveTables;


std::vector<Move> moveGeneration(Position const &board)
{
	std::vector<Move> moveList;
	Color sideToMove = board.getTurn(); // which side are we generating moves for? 
	const Bitboard occupancy = board.getPosition();
	const Bitboard ownPieces = board.getPosition(sideToMove);
	Check check{}; // is the move a check to opponent king?

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
				(sideToMove == WHITE) ? attacks |= MoveTables.whitePawn(C64(1) << squareFrom, ~ownPieces)
					& occupancy : attacks |= MoveTables.blackPawn(C64(1) << squareFrom, ~ownPieces)
					& occupancy;
				// attacks &= occupancy;

				break; } // end case PAWNS
			} // end switch (piece)

			if (piece != PAWN) attacks &= ~ownPieces; // exclude own pieces from attacks

			while (attacks) { // scan the attack bitboard. For each attack, create the move and update the list
				Square squareTo = Square(bitscan_reset(attacks));
				
				check = verifyCheck(Piece(piece), squareTo, board);
				
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
						moveList.push_back(composeMove(squareFrom, squareTo, sideToMove, piece, type, captured, PAWN_TO_KNIGHT, check));
						moveList.push_back(composeMove(squareFrom, squareTo, sideToMove, piece, type, captured, PAWN_TO_QUEEN, check));
					} // end if
					else
						moveList.push_back(composeMove(squareFrom, squareTo, sideToMove, piece, type, captured, 0, check));
			} // end while (attacks)

		} // end while (bb)

	} // end for loop

	
	// ***** CASTLE MOVES, IF AVAILABLE *****
	if (board.getCastle(sideToMove) == QUEENSIDE) {
		if (board.idPiece(A8).piece == ROOK && board.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(A8, occupancy) >> E8) & C64(1))
				moveList.push_back(composeMove(A8, D8, sideToMove, ROOK, CASTLE_Q, NO_PIECE, 0, check));
		}
		else if (board.idPiece(A1).piece == ROOK && board.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(A1, occupancy) >> E1) & C64(1))
				moveList.push_back(composeMove(A1, D1, sideToMove, ROOK, CASTLE_Q, NO_PIECE, 0, check));
		}
	}
	
	else if (board.getCastle(sideToMove) == KINGSIDE) {
		if (board.idPiece(H8).piece == ROOK && board.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(H8, occupancy) >> E8) & C64(1))
				moveList.push_back(composeMove(H8, F8, sideToMove, ROOK, CASTLE_K, NO_PIECE, 0, check));
		}
		else if (board.idPiece(H1).piece == ROOK && board.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(H1, occupancy) >> E1) & C64(1))
				moveList.push_back(composeMove(H1, F1, sideToMove, ROOK, CASTLE_K, NO_PIECE, 0, check));
		}
	}

	else if (board.getCastle(sideToMove) == ALL) {
		if (board.idPiece(A8).piece == ROOK && board.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(A8, occupancy) >> E8) & C64(1))
				moveList.push_back(composeMove(A8, D8, sideToMove, ROOK, CASTLE_Q, NO_PIECE, 0, check));
		}
		else if (board.idPiece(A1).piece == ROOK && board.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(A1, occupancy) >> E1) & C64(1))
				moveList.push_back(composeMove(A1, D1, sideToMove, ROOK, CASTLE_Q, NO_PIECE, 0, check));
		}

		if (board.idPiece(H8).piece == ROOK && board.idPiece(E8).piece == KING) {
			if ((MoveTables.rook(H8, occupancy) >> E8) & C64(1))
				moveList.push_back(composeMove(H8, F8, sideToMove, ROOK, CASTLE_K, NO_PIECE, 0, check));
		}
		else if (board.idPiece(H1).piece == ROOK && board.idPiece(E1).piece == KING) {
			if ((MoveTables.rook(H1, occupancy) >> E1) & C64(1))
				moveList.push_back(composeMove(H1, F1, sideToMove, ROOK, CASTLE_K, NO_PIECE, 0, check));
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
				moveList.push_back(composeMove(squareFrom, enpassant, sideToMove, PAWN, EN_PASSANT, PAWN, 0, check));
			}
			else if (board.idPiece(Square(enpassant - 9)).color == WHITE && board.idPiece(Square(enpassant - 9)).piece == PAWN) {
				Square squareFrom = Square(enpassant - 9);
				moveList.push_back(composeMove(squareFrom, enpassant, sideToMove, PAWN, EN_PASSANT, PAWN, 0, check));
			}
			break;
		case BLACK: // is there a black pawn attacking the en-passant square?
			if (board.idPiece(Square(enpassant + 7)).color == BLACK && board.idPiece(Square(enpassant + 7)).piece == PAWN) {
				Square squareFrom = Square(enpassant + 7);
				moveList.push_back(composeMove(squareFrom, enpassant, sideToMove, PAWN, EN_PASSANT, PAWN, 0, check));
			}
			else if (board.idPiece(Square(enpassant + 9)).color == BLACK && board.idPiece(Square(enpassant + 9)).piece == PAWN) {
				Square squareFrom = Square(enpassant + 9);
				moveList.push_back(composeMove(squareFrom, enpassant, sideToMove, PAWN, EN_PASSANT, PAWN, 0, check));
			}
			break;
		} // end switch		

	} // end main 'if (board.getEnPassant() != SQ_EMPTY)'
	
	return moveList;
}


Check verifyCheck(Piece const &piece, Square const &square, Position const &board) {
	Square opponentKing = Square(bitscan_fwd(board.getPieces(Color(!board.getTurn()), KING)));
	Bitboard attacksToKing{};
	const Bitboard occupancy = board.getPosition();
	const Bitboard ownPieces = board.getPosition(board.getTurn());

	switch (piece) {
	case QUEEN:
		attacksToKing = MoveTables.bishop(square, occupancy)
			| MoveTables.rook(square, occupancy);
		break;
	case ROOK:
		attacksToKing = MoveTables.rook(square, occupancy);
		break;
	case KNIGHT:
		attacksToKing = MoveTables.knight[square];
		break;
	case BISHOP:
		attacksToKing = MoveTables.bishop(square, occupancy);
		break;
	case PAWN: 
		(board.getTurn() == WHITE) ? attacksToKing |= MoveTables.whitePawn(C64(1) << square, occupancy)
			: attacksToKing |= MoveTables.blackPawn(C64(1) << square, occupancy);
		break; 
	}

	while (attacksToKing) {
		Square squareTo = Square(bitscan_reset(attacksToKing));
		if (ushort(squareTo) == ushort(opponentKing)) return CHECK;
	}

	return NO_CHECK;
}


Move composeMove(Square const &squareFrom, Square const &squareTo,
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


void doMove(Move const m, Position &p)
{
	Square squareFrom{}, squareTo{};
	ushort moveType = ((C64(1) << 3) - 1) & (m >> 5);
	squareFrom = Square(((C64(1) << 6) - 1) & (m >> 18));
	squareTo = Square(((C64(1) << 6) - 1) & (m >> 12));
	Color color = Color(((C64(1) << 1) - 1) & (m >> 11));
	Piece piece = Piece(((C64(1) << 3) - 1) & (m >> 8));
	Piece captured = Piece(((C64(1) << 3) - 1) & (m >> 2));
	bool promotedTo = ((C64(1) << 1) - 1) & (m >> 1);
	bool check = ((C64(1) << 1) - 1) & (m);

	switch (moveType)
	{ 
	case QUIET:
		p.removePiece(color, piece, squareFrom);
		p.putPiece(color, piece, squareTo);

		if ((piece == PAWN && (squareFrom / 8) == RANK_2 && (squareTo / 8) == RANK_4)
			|| (piece == PAWN && (squareFrom / 8) == RANK_7 && (squareTo / 8) == RANK_5))
			(color == WHITE) ? p.setEnPassant(Square(squareTo - 8)) : p.setEnPassant(Square(squareTo + 8));
		
		if (color == BLACK) 
			p.setMoveNumber(p.getMoveNumber() + 1);
		
		p.setHalfMove(p.getHalfMove() + 1);
		p.setTurn(Color(!p.getTurn()));
		break;
	case CAPTURE:
		p.removePiece(color, piece, squareFrom);
		p.removePiece(Color(!color), captured, squareTo);
		p.putPiece(color, piece, squareTo);
		
		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);
		
		p.setHalfMove(0);
		p.setTurn(Color(!p.getTurn()));
		break;
	case PROMOTION: 
		p.removePiece(color, piece, squareTo);
		if (captured != NO_PIECE) {
			p.removePiece(Color(!color), captured, squareTo);
			p.setHalfMove(0);
		}
		else
			p.setHalfMove(p.getHalfMove() - 1);

		(promotedTo == PAWN_TO_KNIGHT) ? p.putPiece(color, KNIGHT, squareTo) : p.putPiece(color, QUEEN, squareTo);

		p.setTurn(Color(!p.getTurn()));
		break;
	case CASTLE_Q: {
		Square kingPos = p.getPieceOnSquare(color, KING)[0];
		p.removePiece(color, piece, squareFrom);
		p.removePiece(color, KING, Square(kingPos));
		p.putPiece(color, piece, squareTo);
		p.putPiece(color, KING, Square(kingPos -2));

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(p.getHalfMove() + 1);
		p.setTurn(Color(!p.getTurn()));
		break;	}
	case CASTLE_K: {
		Square kingPos = p.getPieceOnSquare(color, KING)[0];
		p.removePiece(color, piece, squareFrom);
		p.removePiece(color, KING, Square(kingPos));
		p.putPiece(color, piece, squareTo);
		p.putPiece(color, KING, Square(kingPos + 2));

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(p.getHalfMove() + 1);
		p.setTurn(Color(!p.getTurn()));
		break; }
	case EN_PASSANT:
		p.removePiece(color, piece, squareFrom);
		
		(color == WHITE) ? p.removePiece(color, PAWN, Square(squareTo - 8))
			: p.removePiece(color, PAWN, Square(squareTo + 8));
		
		p.putPiece(color, piece, squareTo);
		
		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);
		
		p.setHalfMove(0);
		p.setTurn(Color(!p.getTurn()));
		break;
	}


	if (squareTo != p.getEnPassant())
		p.setEnPassant(SQ_EMPTY);
}


void undoMove(Move const m, Position &p)
{
	Square squareFrom{}, squareTo{};
	ushort moveType = ((C64(1) << 3) - 1) & (m >> 5);
	squareFrom = Square(((C64(1) << 6) - 1) & (m >> 18));
	squareTo = Square(((C64(1) << 6) - 1) & (m >> 12));
	Color color = Color(((C64(1) << 1) - 1) & (m >> 11));
	Piece piece = Piece(((C64(1) << 3) - 1) & (m >> 8));
	Piece captured = Piece(((C64(1) << 3) - 1) & (m >> 2));
	bool promotedTo = ((C64(1) << 1) - 1) & (m >> 1);
	bool check = ((C64(1) << 1) - 1) & (m);

	switch (moveType)
	{
	case QUIET:
		p.removePiece(color, piece, squareTo);
		p.putPiece(color, piece, squareFrom);

		if ((piece == PAWN && (squareTo / 8) == RANK_4 && (squareFrom / 8) == RANK_2)
			|| (piece == PAWN && (squareTo / 8) == RANK_5 && (squareFrom / 8) == RANK_7))
			p.setEnPassant(SQ_EMPTY);

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() - 1);

		p.setHalfMove(p.getHalfMove() - 1);
		p.setTurn(Color(!p.getTurn()));
		break;
	case CAPTURE:
		p.removePiece(color, piece, squareTo);
		p.putPiece(Color(!color), captured, squareTo);
		p.putPiece(color, piece, squareFrom);

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() - 1);

		p.setHalfMove(0); // PROBLEM. HOW DO WE RESTORE THIS???????
		p.setTurn(Color(!p.getTurn()));
		break;
	case PROMOTION: 
		(promotedTo == PAWN_TO_KNIGHT) ? p.removePiece(color, KNIGHT, squareTo) : p.removePiece(color, QUEEN, squareTo);
		if (captured != NO_PIECE) p.putPiece(color, captured, squareTo);
		p.putPiece(color, PAWN, squareFrom);
		
		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() - 1);
		
		p.setTurn(Color(!p.getTurn()));
		break; 
	case CASTLE_Q: {
		Square kingPos = p.getPieceOnSquare(color, KING)[0];
		p.removePiece(color, piece, squareTo);
		p.removePiece(color, KING, Square(kingPos));
		p.putPiece(color, piece, squareFrom);
		p.putPiece(color, KING, Square(kingPos + 2));

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() - 1);

		p.setHalfMove(p.getHalfMove() - 1);
		p.setTurn(Color(!p.getTurn()));
		break; }
	case CASTLE_K: {
		Square kingPos = p.getPieceOnSquare(color, KING)[0];
		p.removePiece(color, piece, squareTo);
		p.removePiece(color, KING, Square(kingPos));
		p.putPiece(color, piece, squareFrom);
		p.putPiece(color, KING, Square(kingPos - 2));

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() - 1);

		p.setHalfMove(p.getHalfMove() - 1);
		p.setTurn(Color(!p.getTurn()));
		break; }
	case EN_PASSANT:
		p.removePiece(color, piece, squareTo);

		(color == WHITE) ? p.putPiece(color, PAWN, Square(squareTo - 8))
			: p.putPiece(color, PAWN, Square(squareTo + 8));

		p.putPiece(color, piece, squareFrom);

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() - 1);

		p.setHalfMove(0); // PROBLEM!!! WHERE DO WE RESTORE THIS FROM????
		(color == WHITE) ? p.setEnPassant(Square(squareTo - 8)) : p.setEnPassant(Square(squareTo + 8));
		p.setTurn(Color(!p.getTurn()));
		break;
	}
}