#include "pch.h"
#include "Position.h"


void doMove(Move const &m, Position &p)
{
	Square squareFrom{}, squareTo{};
	ushort moveType = ((C64(1) << 3) - 1) & (m >> 6);
	squareFrom = Square(((C64(1) << 6) - 1) & (m >> 19));
	squareTo = Square(((C64(1) << 6) - 1) & (m >> 13));
	Color color = Color(((C64(1) << 1) - 1) & (m >> 12));
	Piece piece = Piece(((C64(1) << 3) - 1) & (m >> 9));
	Piece captured = Piece(((C64(1) << 3) - 1) & (m >> 3));
	ushort promotedTo = ((C64(1) << 3) - 1) & (m);

	switch (moveType)
	{
	case QUIET:
		p.removePiece(color, piece, squareFrom);
		p.putPiece(color, piece, squareTo);

		if (	(piece == PAWN) && ((squareFrom / 8) == RANK_2) && ((squareTo / 8) == RANK_4)
			|| (piece == PAWN) && ((squareFrom / 8) == RANK_7) && ((squareTo / 8) == RANK_5)	)
			(color == WHITE) ? p.setEnPassant(Square(squareTo - 8)) : p.setEnPassant(Square(squareTo + 8));

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(p.getHalfMove() + 1);
		p.setTurn(Color(!color));
		break;
	case CAPTURE:
		p.removePiece(color, piece, squareFrom);
		p.removePiece(Color(!color), captured, squareTo);
		p.putPiece(color, piece, squareTo);
		
		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(0);
		p.setTurn(Color(!color));
		break;
	case PROMOTION:
		p.removePiece(color, piece, squareFrom);
		
		if (!(captured == NO_PIECE)) {
			p.removePiece(Color(!color), captured, squareTo);
			p.setHalfMove(0);
		}
		else
			p.setHalfMove(p.getHalfMove() + 1);
			

		switch (promotedTo) {
		case PAWN_TO_KNIGHT:
			p.putPiece(color, KNIGHT, squareTo);
			break;
		case PAWN_TO_QUEEN:
			p.putPiece(color, QUEEN, squareTo);
			break;
		case PAWN_TO_BISHOP:
			p.putPiece(color, BISHOP, squareTo);
			break;
		case PAWN_TO_ROOK:
			p.putPiece(color, ROOK, squareTo);
			break;
		}	

		p.setTurn(Color(!color));
		break;
	case CASTLE_Q: {
		p.removePiece(color, piece, squareFrom);
		(color == WHITE) ? p.removePiece(color, ROOK, A1) : p.removePiece(color, ROOK, A8);
		p.putPiece(color, piece, squareTo);
		(color == WHITE) ? p.putPiece(color, ROOK, D1) : p.putPiece(color, ROOK, D8); 

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(p.getHalfMove() + 1);
		p.setCastle(color, NONE);
		p.setTurn(Color(!color));
		break;	}
	case CASTLE_K: {
		p.removePiece(color, piece, squareFrom);
		(color == WHITE) ? p.removePiece(color, ROOK, H1) : p.removePiece(color, ROOK, H8);
		p.putPiece(color, piece, squareTo);
		(color == WHITE) ? p.putPiece(color, ROOK, F1) : p.putPiece(color, ROOK, F8);

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(p.getHalfMove() + 1);
		p.setCastle(color, NONE);
		p.setTurn(Color(!color));
		break; }
	case EN_PASSANT:
		p.removePiece(color, piece, squareFrom);

		(color == WHITE) ? p.removePiece(Color(!color), PAWN, Square(squareTo - 8))
			: p.removePiece(Color(!color), PAWN, Square(squareTo + 8));

		p.putPiece(color, piece, squareTo);

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(0);
		p.setEnPassant(SQ_EMPTY);
		p.setTurn(Color(!color));
		break;
	}

	if (!(moveType == EN_PASSANT) && !(p.getEnPassant() == SQ_EMPTY)) {
		Color temp;
		(p.getEnPassant() / 8 == RANK_3) ? temp = WHITE : temp = BLACK;
		if (!(temp == color)) 
			p.setEnPassant(SQ_EMPTY);
	}

	// if King moves, no more castle
	if (!(moveType == CASTLE_Q) && !(moveType == CASTLE_K) && (piece == KING))
		p.setCastle(color, NONE);

	// if Rook moves, no more castle on that side
	switch (color)
	{
	case WHITE:
		if (	!(moveType == CASTLE_K) && piece == ROOK && squareFrom == H1
			&& (p.getCastle(WHITE) == KINGSIDE || p.getCastle(WHITE) == ALL))
			p.setCastle(WHITE, p.getCastle(WHITE) - KINGSIDE);

		if (	!(moveType == CASTLE_Q) && piece == ROOK && squareFrom == A1
			&& (p.getCastle(WHITE) == QUEENSIDE || p.getCastle(WHITE) == ALL)	)
			p.setCastle(WHITE, p.getCastle(WHITE) - QUEENSIDE);

		break;
	case BLACK:
		if (!(moveType == CASTLE_K) && piece == ROOK && squareFrom == H8
			&& (p.getCastle(BLACK) == KINGSIDE || p.getCastle(BLACK) == ALL))
			p.setCastle(BLACK, p.getCastle(BLACK) - KINGSIDE);

		if (	!(moveType == CASTLE_Q) && piece == ROOK && squareFrom == A8
			&& (p.getCastle(BLACK) == QUEENSIDE || p.getCastle(BLACK) == ALL)	)
			p.setCastle(BLACK, p.getCastle(BLACK) - QUEENSIDE);

		break;
	}

	// if Rook gets captured, no more castle, if previously possible
	if (color == BLACK && captured == ROOK && squareTo == H1 
		&& (	p.getCastle(WHITE) == KINGSIDE || p.getCastle(WHITE) == ALL))
		p.setCastle(WHITE, p.getCastle(WHITE) - KINGSIDE);
	
	else if (color == BLACK && captured == ROOK && squareTo == A1
		&& (	p.getCastle(WHITE) == QUEENSIDE || p.getCastle(WHITE) == ALL))
		p.setCastle(WHITE, p.getCastle(WHITE) - QUEENSIDE);
	
	else if (color == WHITE && captured == ROOK && squareTo == H8 
		&& (p.getCastle(BLACK) == KINGSIDE || p.getCastle(BLACK) == ALL))
		p.setCastle(BLACK, p.getCastle(BLACK) - KINGSIDE);
	
	else if (color == WHITE && captured == ROOK && squareTo == A8 
		&& (p.getCastle(BLACK) == KINGSIDE || p.getCastle(BLACK) == ALL))
		p.setCastle(BLACK, p.getCastle(BLACK) - QUEENSIDE);
}


void undoMove(Move const &m, Position &p, Position const &backup)
{
	p = backup;
}