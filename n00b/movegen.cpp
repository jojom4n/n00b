#include "pch.h"
#include "movegen.h"
#include "attack.h"
#include "bitscan.h"
#include "makemove.h"
#include "overloading.h"
#include "Position.h"

extern struct LookupTable g_MoveTables; // see attack.cpp (and its header file)

const std::vector<Move> moveGeneration(Position const &p)
{
	Color sideToMove = p.getTurn(); // which side are we generating moves for? 
	
	if (underCheck(sideToMove, p) > 1) {
		return generateOnlyKing(sideToMove, p); // if King is under double attack, generate only king evasions
	}

	std::vector<Move> moveList;
	moveList.reserve(MAX_PLY);

	const Bitboard occupancy = p.getPosition();
	const Bitboard ownPieces = p.getPosition(sideToMove);
	
	for (Piece piece = KING; piece <= PAWN; piece++) {
		Bitboard bb = p.getPieces(sideToMove, piece);
		
		while (bb) { // loop until the bitboard has a piece on it
			Square squareFrom = Square(bitscan_reset(bb)); // find the square(s) with the particular piece;
			Bitboard moves{};
			
			// get move bitboard for the piece, given its square
			switch (piece) {
			case KING:
				moves = g_MoveTables.king[squareFrom];
				break;
			case QUEEN:
				moves = g_MoveTables.bishop(squareFrom, occupancy)
					| g_MoveTables.rook(squareFrom, occupancy);
				break;
			case ROOK:
				moves = g_MoveTables.rook(squareFrom, occupancy);
				break;
			case KNIGHT:
				moves = g_MoveTables.knight[squareFrom];
				break;
			case BISHOP:
				moves = g_MoveTables.bishop(squareFrom, occupancy);
				break;
			case PAWN: 
				/* For pawns, it's a bit more complicated, because they have no lookup tables.
				For readibility, better pass the params to an appropriate function */
				moves = pawnMoves(p, squareFrom);	
				break;
			}

			if (!(piece == PAWN))
				moves &= ~ownPieces; // exclude own pieces from moves
			
			while (moves) { // scan collected moves, determine their type and add them to list
				Square squareTo = Square(bitscan_reset(moves));
				MoveType type = setType(piece, occupancy, sideToMove, squareFrom, squareTo);

				Piece captured = p.idPiece(squareTo, Color(!(sideToMove))).piece;
				
				if (captured == KING) break; // captured can't be enemy king

				if (type == CAPTURE) {
					Move m = composeMove(squareFrom, squareTo, sideToMove, piece, type, captured, 0);
					moveList.push_back(m);
				}
				else if (type == PROMOTION) { 
					// compose one moves for each possible promotion
					Move m = composeMove(squareFrom, squareTo, sideToMove, piece, type, captured, PAWN_TO_KNIGHT);
					Move m2 = composeMove(squareFrom, squareTo, sideToMove, piece, type, captured, PAWN_TO_QUEEN);
					Move m3 = composeMove(squareFrom, squareTo, sideToMove, piece, type, captured, PAWN_TO_ROOK);
					Move m4 = composeMove(squareFrom, squareTo, sideToMove, piece, type, captured, PAWN_TO_BISHOP);
					moveList.push_back(m);
					moveList.push_back(m2);
					moveList.push_back(m3);
					moveList.push_back(m4);
				}
				else {
					Move m = composeMove(squareFrom, squareTo, sideToMove, piece, type, captured, 0);
					moveList.push_back(m);
				}
			} // end while (moves)
		} // end while (bb)
	} // end for loop

	if (!(underCheck(sideToMove, p))) // if King is not under check, calculate castles too
		castleMoves(p, moveList);

	if (!(p.getEnPassant() == SQ_EMPTY))
		enPassant(p, p.getEnPassant(), sideToMove, moveList);

	return moveList;
}


const std::vector<Move> generateOnlyKing(Color const &c, Position const &p)
{
	std::vector<Move> moveList;
	moveList.reserve(MAX_PLY);
	const Bitboard occ = p.getPosition(), ownPieces = p.getPosition(c);
	Bitboard moves{};
	Square kingPos = p.getPieceOnSquare(c, KING)[0];
	moves = g_MoveTables.king[kingPos] & ~ownPieces;

	while (moves) { // scan collected moves, determine their type and add them to list
		Square squareTo = Square(bitscan_reset(moves));
		MoveType type = setType(KING, occ, c, kingPos, squareTo);
		Piece captured = p.idPiece(squareTo, Color(!p.getTurn())).piece;

		if (type == CAPTURE) {
			Move m = composeMove(kingPos, squareTo, c, KING, type, captured, 0);
			moveList.push_back(m);
		}
		else {
			Move m = composeMove(kingPos, squareTo, c, KING, type, captured, 0);
			moveList.push_back(m);
		}
	}

	return moveList;
}


const Bitboard pawnMoves(Position const &p, Square const &from)
{
	Color c = p.getTurn();
	Bitboard m{}, occ = p.getPosition(), own = p.getPosition(c);
	
	(c == WHITE) ? m |= ((C64(1) << from) << 8) & ~occ
		: m |= ((C64(1) << from) >> 8) & ~occ;

	// Is the pawn on 2nd rank(or 7th rank, for black)? If yes, let it move even two squares ahead
	if (c == WHITE && (from / 8) == RANK_2) 
		m ^= (m << 8) & ~occ;
	else if (c == BLACK && (from / 8) == RANK_7)
		m ^= (m >> 8) & ~occ;
		
	/* Set in an empty bitboard the bit corresponding to 'from' (i.e. where pawn is)
	and compute if it can attack from there opponent's pieces*/
	(c == WHITE) ? m |= g_MoveTables.whitePawn(C64(1) << from, ~own) & occ
		: m |= g_MoveTables.blackPawn(C64(1) << from, ~own) & occ;

	return m;
}


void castleMoves(Position const &p, std::vector<Move> &moveList)
{
	Move m{};
	Color c = p.getTurn();
	Bitboard occ = p.getPosition();

	if (p.getCastle(c) == Castle::QUEENSIDE || p.getCastle(c) == Castle::ALL)
		if (	(p.idPiece(A8, c).piece == ROOK && p.idPiece(E8, c).piece == KING) // rook and king in position
			&& (	(g_MoveTables.rook(A8, occ) >> E8) & C64(1)	) // no pieces between rook and king
			&& (	!(p.isSquareAttackedBy(Color(!c), C8))	)   // C8 and D8 must not be under attack
			&& (	!(p.isSquareAttackedBy(Color(!c), D8))	)	)
		{  
			m = composeMove(E8, C8, c, KING, CASTLE_Q, NO_PIECE, 0);
			moveList.push_back(m);
		}

		else if (	(p.idPiece(A1, c).piece == ROOK && p.idPiece(E1, c).piece == KING)  // rook and king in position
			&& (	(g_MoveTables.rook(A1, occ) >> E1) & C64(1)	)  // no pieces between rook and king
			&& (	!(p.isSquareAttackedBy(Color(!c), C1))	)   // C1 and D1 must not be under attack
			&& (	!(p.isSquareAttackedBy(Color(!c), D1))	)	)
		{
			m = composeMove(E1, C1, c, KING, CASTLE_Q, NO_PIECE, 0);
			moveList.push_back(m);
		}

	if (p.getCastle(c) == Castle::KINGSIDE || p.getCastle(c) == Castle::ALL)
		if (	(p.idPiece(H8, c).piece == ROOK && p.idPiece(E8, c).piece == KING)  // rook and king in position
			&& (	(g_MoveTables.rook(H8, occ) >> E8) & C64(1)	)  // no pieces between rook and king
			&& (	!(p.isSquareAttackedBy(Color(!c), F8))	)   // F8 and G8 must not be under attack
			&& (	!(p.isSquareAttackedBy(Color(!c), G8))	)	)
		{
			m = composeMove(E8, G8, c, KING, CASTLE_K, NO_PIECE, 0);
			moveList.push_back(m);
		}

		else if (	(p.idPiece(H1, c).piece == ROOK && p.idPiece(E1, c).piece == KING) // rook and king in position
			&& (	(g_MoveTables.rook(H1, occ) >> E1) & C64(1)	)   // no pieces between rook and king
			&& (	!(p.isSquareAttackedBy(Color(!c), F1))	)   // F1 and G1 must not be under attack
			&& (	!(p.isSquareAttackedBy(Color(!c), G1))	)	)
		{
			m = composeMove(E1, G1, c, KING, CASTLE_K, NO_PIECE, 0);
			moveList.push_back(m);
		}
}


void enPassant(Position const &p, Square const &enPassant, Color const &c, std::vector<Move>& moveList)
{
	Move m{};

	if (!(enPassant % 8) == FILE_A && !((enPassant % 8) == FILE_H))
		switch (c) {
		case WHITE: { // is there a white pawn attacking the en-passant square?
			PieceID probablePawn = p.idPiece(Square(enPassant - 7), c);

			if (probablePawn.color == WHITE && probablePawn.piece == PAWN) {
				Square squareFrom = Square(enPassant - 7);
				Bitboard attacksToKing{}, occ = p.getPosition();
				attacksToKing |= g_MoveTables.whitePawn(C64(1) << enPassant, occ);
				m = composeMove(squareFrom, enPassant, c, PAWN, EN_PASSANT, PAWN, 0);
				moveList.push_back(m);
			}

			probablePawn = p.idPiece(Square(enPassant - 9), c);

			if (probablePawn.color == WHITE && probablePawn.piece == PAWN) {
				Square squareFrom = Square(enPassant - 9);
				Bitboard attacksToKing{}, occ = p.getPosition();
				attacksToKing |= g_MoveTables.whitePawn(C64(1) << enPassant, occ);
				m = composeMove(squareFrom, enPassant, c, PAWN, EN_PASSANT, PAWN, 0);
				moveList.push_back(m);
			}
			break; }
		case BLACK: { // is there a black pawn attacking the en-passant square?
			PieceID probablePawn = p.idPiece(Square(enPassant + 7), c);

			if (probablePawn.color == BLACK && probablePawn.piece == PAWN) {
				Square squareFrom = Square(enPassant + 7);
				Bitboard attacksToKing{}, occ = p.getPosition();
				attacksToKing |= g_MoveTables.blackPawn(C64(1) << enPassant, occ);
				m = composeMove(squareFrom, enPassant, c, PAWN, EN_PASSANT, PAWN, 0);
				moveList.push_back(m);
			}

			probablePawn = p.idPiece(Square(enPassant + 9), c);

			if (probablePawn.color == BLACK && probablePawn.piece == PAWN) {
				Square squareFrom = Square(enPassant + 9);
				Bitboard attacksToKing{}, occ = p.getPosition();
				attacksToKing |= g_MoveTables.blackPawn(C64(1) << enPassant, occ);
				m = composeMove(squareFrom, enPassant, c, PAWN, EN_PASSANT, PAWN, 0);
				moveList.push_back(m);
			}
			break; }
		} // end switch	

	if ((enPassant %8) == FILE_A)
		switch (c) {
		case WHITE: { // is there a white pawn attacking the en-passant square?
			PieceID probablePawn = p.idPiece(Square(enPassant - 7), c);
			if (probablePawn.color == WHITE && probablePawn.piece == PAWN) {
				Square squareFrom = Square(enPassant - 7);
				Bitboard attacksToKing{}, occ = p.getPosition();
				attacksToKing |= g_MoveTables.whitePawn(C64(1) << enPassant, occ);
				m = composeMove(squareFrom, enPassant, c, PAWN, EN_PASSANT, PAWN, 0);
				moveList.push_back(m);
			}
			break; }
		case BLACK: { // is there a black pawn attacking the en-passant square?
			PieceID probablePawn = p.idPiece(Square(enPassant + 9), c);
			if (probablePawn.color == BLACK && probablePawn.piece == PAWN) {
				Square squareFrom = Square(enPassant + 9);
				Bitboard attacksToKing{}, occ = p.getPosition();
				attacksToKing |= g_MoveTables.blackPawn(C64(1) << enPassant, occ);
				m = composeMove(squareFrom, enPassant, c, PAWN, EN_PASSANT, PAWN, 0);
				moveList.push_back(m);
			}
			break; }
		} // end switch	

	if ((enPassant % 8 == FILE_H))
		switch (c) {
		case WHITE: { // is there a white pawn attacking the en-passant square?
			PieceID probablePawn = p.idPiece(Square(enPassant - 9), c);
			if (probablePawn.color == WHITE && probablePawn.piece == PAWN) {
				Square squareFrom = Square(enPassant - 9);
				Bitboard attacksToKing{}, occ = p.getPosition();
				attacksToKing |= g_MoveTables.whitePawn(C64(1) << enPassant, occ);
				m = composeMove(squareFrom, enPassant, c, PAWN, EN_PASSANT, PAWN, 0);
				moveList.push_back(m);
			}
			break; }
		case BLACK: { // is there a black pawn attacking the en-passant square?
			PieceID probablePawn = p.idPiece(Square(enPassant + 7), c);
			if (probablePawn.color == BLACK && probablePawn.piece == PAWN) {
				Square squareFrom = Square(enPassant + 7);
				Bitboard attacksToKing{}, occ = p.getPosition();
				attacksToKing |= g_MoveTables.blackPawn(C64(1) << enPassant, occ);
				m = composeMove(squareFrom, enPassant, c, PAWN, EN_PASSANT, PAWN, 0);
				moveList.push_back(m);
			}
			break; }
		} // end switch	
}


const MoveType setType(Piece const &piece, Bitboard const &occ, Color const &c, Square const &from, Square const &to)
{
	MoveType type;
	
	if (	(piece == PAWN && c == WHITE && (from / 8) == RANK_7 && (to / 8) == RANK_8)	
		||	(piece == PAWN && c == BLACK && (from / 8) == RANK_2 && (to / 8) == RANK_1)	)
		type = PROMOTION; // is the move a pawn promotion?
	else if ((occ >> to) & 1ULL)
		type = CAPTURE; // if destination square is occupied by enemy...
	else
		type = QUIET;

	return type;
}


const Move composeMove(Square const &from, Square const &to, Color const &c, ushort const &p, 
	MoveType const &type, Piece const &captured, ushort const &promoteTo)
{
	
	/* combine from, to and type into a binary number
	We use a 32-bit number (Move = uint32_t, see defs.h), composed in the following way:

	0000000   	000000		 000000		  0		 000	    000			     000			   000		
	 unused      from 		   to		Color	Piece	Type of move	Piece captured,    Promotion  
																			if any			 to...
	
	*/

	Move move{};
	move = (move << 6) | from;
	move = (move << 6) | to;
	move = (move << 1) | c;
	move = (move << 3) | p;
	move = (move << 3) | type;
	move = (move << 3) | captured;
	move = (move << 3) | promoteTo;
	
	// update the list of moves with the new move
	return move;
}


short underCheck(Color const &c, Position const &p)
{
	Square kingPos = p.getPieceOnSquare(c, KING)[0]; //get King's square
	Bitboard kingBB = p.getPieces(c, KING);
	Bitboard opponent{}, attackedBy{}, occ = p.getPosition();
	short attackers{};
	
	// ROOK
	opponent = p.getPieces(Color(!c), ROOK); // get rook's bitboard
	if (opponent) {
		attackedBy = g_MoveTables.rook(kingPos, occ); // does rook's attack mask...
		if (attackedBy &= opponent) // ...intersect King's square?
			attackers += 1;
	}
	
	// BISHOP
	opponent = p.getPieces(Color(!c), BISHOP); // get bishop's bitboard
	if (opponent) {
		attackedBy = g_MoveTables.bishop(kingPos, occ); // does bishop's attack mask...
		if (attackedBy &= opponent) // ...intersect King's square?
			attackers += 1;
	}

	// QUEEN
	opponent = p.getPieces(Color(!c), QUEEN); // get queen's bitboard
	if (opponent) {
		attackedBy = g_MoveTables.rook(kingPos, occ) | g_MoveTables.bishop(kingPos, occ); // does queen's attack mask...
		if (attackedBy &= opponent) // ...intersect King's square?
			attackers += 1;
	}


	// KNIGHT
	opponent = p.getPieces(Color(!c), KNIGHT); // get knight's bitboard
	if (opponent) {
		attackedBy = g_MoveTables.knight[kingPos]; // does knight's attack mask...
		if (attackedBy &= opponent) // ...intersect King's square?
			attackers += 1;
	}

	// KING
	opponent = p.getPieces(Color(!c), KING); // get enemy king's bitboard
	attackedBy = g_MoveTables.king[kingPos]; // does enemy king's attack mask...
	if (attackedBy &= opponent) // ...intersect King's square?
		attackers += 1;

	//PAWNS
	opponent = p.getPieces(Color(!c), PAWN); // get pawn's bitboard
	if (opponent) {
		// does enemy pawn's attack mask...
		(c == WHITE) ? attackedBy = g_MoveTables.blackPawn(opponent, occ) : attackedBy = g_MoveTables.whitePawn(opponent, occ);
		if (attackedBy &= kingBB) // ...intersect King's square?
			attackers += 1;
	}

	return attackers; // return number of pieces attacking the King
}


const std::vector<Move> pruneIllegal (std::vector<Move> &moveList, Position &p)
{
	p.storeState();

	
	for (auto it = moveList.begin(); it != moveList.end(); ) // scroll through the moveList
	{
		Color c = Color(((C64(1) << 1) - 1) & (*it >> 12)); // who's moving?
		doMove(*it, p); // do the move
		
		if (underCheck(c, p)) { // if move is not legal...
			undoMove(*it, p); // undo the move...
			p.restoreState();
			it = moveList.erase(it); // and erase it from moveList
		}
		else {
			undoMove(*it, p); // else undo move...
			p.restoreState();
			it++; // ...retain the move, because it's valid, and check next one
		} // end if
	} // end for

	return moveList;
}


const std::vector<Move> moveGenQS(Position const& p)
{
	std::vector<Move> moveList;
	moveList.reserve(MAX_PLY);

	Color sideToMove = p.getTurn(); // which side are we generating moves for? 

	if (underCheck(sideToMove, p) > 1)
		return generateOnlyKing(sideToMove, p); // if King is under double attack, generate only king evasions

	const Bitboard occ = p.getPosition();
	const Bitboard own = p.getPosition(sideToMove);

	for (Piece piece = KING; piece <= PAWN; piece++) {
		Bitboard bb = p.getPieces(sideToMove, piece);

		while (bb) { // loop until the bitboard has a piece on it
			Square squareFrom = Square(bitscan_reset(bb)); // find the square(s) with the particular piece;
			Bitboard m{};

			// get move bitboard for the piece, given its square
			switch (piece) {
			case KING:
				m = g_MoveTables.king[squareFrom];
				break;
			case QUEEN:
				m = g_MoveTables.bishop(squareFrom, occ)
					| g_MoveTables.rook(squareFrom, occ);
				break;
			case ROOK:
				m = g_MoveTables.rook(squareFrom, occ);
				break;
			case KNIGHT:
				m = g_MoveTables.knight[squareFrom];
				break;
			case BISHOP:
				m = g_MoveTables.bishop(squareFrom, occ);
				break;
			case PAWN:
				(sideToMove == WHITE) ? m |= g_MoveTables.whitePawn(C64(1) << squareFrom, occ) 
					: m |= g_MoveTables.blackPawn(C64(1) << squareFrom, occ);
				break;
			}
	
			// now let's add captures to movelist
			m &= occ; // select only bits corresponding to captures
			m &= ~own; // exclude own pieces from moves			

			while (m) { // scan collected captures and add them to list
				Square squareTo = Square(bitscan_reset(m));
				Piece captured = p.idPiece(squareTo, Color(!sideToMove)).piece;

				if (captured == KING) break; // captured can't be enemy king

				moveList.push_back(composeMove(squareFrom, squareTo, sideToMove, piece, CAPTURE, captured, 0));
				
			} // end while (moves)


		} // end while (bb)
	} // end for loop

	return moveList;
}
