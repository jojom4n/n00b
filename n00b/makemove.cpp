#include "pch.h"
#include "protos.h"
#include "Position.h"

extern struct LookupTable MoveTables;

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

		p.updateZobrist(color, piece, squareFrom);
		p.updateZobrist(color, piece, squareTo);

		if ((piece == PAWN) && ((squareFrom / 8) == RANK_2) && ((squareTo / 8) == RANK_4)
			|| (piece == PAWN) && ((squareFrom / 8) == RANK_7) && ((squareTo / 8) == RANK_5))
			switch (color)
			{
			case WHITE: {
				Bitboard ep = MoveTables.blackPawn(p.getPieces(BLACK, PAWN), C64(1) << (squareTo - 8));
				if (ep) {
					p.updateZobrist(p.getEnPassant());
					p.setEnPassant(Square(squareTo - 8));
					p.updateZobrist(p.getEnPassant());
				}
				break; }
			case BLACK: {
				Bitboard ep = MoveTables.whitePawn(p.getPieces(WHITE, PAWN), C64(1) << (squareTo + 8));
				if (ep) {
					p.updateZobrist(p.getEnPassant());
					p.setEnPassant(Square(squareTo + 8));
					p.updateZobrist(p.getEnPassant());
				}
				break; }
			}

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(p.getHalfMove() + 1);
		p.updateZobrist(p.getTurn());
		p.setTurn(Color(!color));
		p.updateZobrist(p.getTurn());

		break;
	case CAPTURE:
		p.removePiece(color, piece, squareFrom);
		p.removePiece(Color(!color), captured, squareTo);
		p.putPiece(color, piece, squareTo);
		
		p.updateZobrist(color, piece, squareFrom);
		p.updateZobrist(Color(!color), captured, squareTo);
		p.updateZobrist(color, piece, squareTo);

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(0);
		p.updateZobrist(p.getTurn());
		p.setTurn(Color(!color));
		p.updateZobrist(p.getTurn());

		break;
	case PROMOTION:
		p.removePiece(color, piece, squareFrom);
		p.updateZobrist(color, piece, squareFrom);
		
		if (!(captured == NO_PIECE)) {
			p.removePiece(Color(!color), captured, squareTo);
			p.setHalfMove(0);
			p.updateZobrist(Color(!color), captured, squareTo);
		}
		else
			p.setHalfMove(p.getHalfMove() + 1);
			
		switch (promotedTo) {
		case PAWN_TO_KNIGHT:
			p.putPiece(color, KNIGHT, squareTo);
			p.updateZobrist(color, KNIGHT, squareTo);
			break;
		case PAWN_TO_QUEEN:
			p.putPiece(color, QUEEN, squareTo);
			p.updateZobrist(color, QUEEN, squareTo);
			break;
		case PAWN_TO_BISHOP:
			p.putPiece(color, BISHOP, squareTo);
			p.updateZobrist(color, BISHOP, squareTo);
			break;
		case PAWN_TO_ROOK:
			p.putPiece(color, ROOK, squareTo);
			p.updateZobrist(color, ROOK, squareTo);
			break;
		}	

		p.updateZobrist(p.getTurn());
		p.setTurn(Color(!color));
		p.updateZobrist(p.getTurn());

		break;
	case CASTLE_Q: {
		p.removePiece(color, piece, squareFrom);
		p.updateZobrist(color, piece, squareFrom);

		if (color == WHITE) {
			p.removePiece(color, ROOK, A1);
			p.updateZobrist(color, ROOK, A1);
		}
		else {
			p.removePiece(color, ROOK, A8);
			p.updateZobrist(color, ROOK, A8);
		}
				
		p.putPiece(color, piece, squareTo);
		p.updateZobrist(color, piece, squareTo);

		if (color == WHITE) {
			p.putPiece(color, ROOK, D1);
			p.updateZobrist(color, ROOK, D1);
		}
		else {
			p.putPiece(color, ROOK, D8);
			p.updateZobrist(color, ROOK, D8);
		}

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(p.getHalfMove() + 1);

		p.updateZobrist(color, p.getCastle(color));
		p.setCastle(color, Castle::NONE);
		p.updateZobrist(color, Castle::NONE);
		
		p.updateZobrist(p.getTurn());
		p.setTurn(Color(!color));
		p.updateZobrist(p.getTurn());

		break;	}
	case CASTLE_K: {
		p.removePiece(color, piece, squareFrom);
		p.updateZobrist(color, piece, squareFrom);
		
		if (color == WHITE) {
			p.removePiece(color, ROOK, H1);
			p.updateZobrist(color, ROOK, H1);
		}
		else {
			p.removePiece(color, ROOK, H8);
			p.updateZobrist(color, ROOK, H8);
		}

		p.putPiece(color, piece, squareTo);
		p.updateZobrist(color, piece, squareTo);

		if (color == WHITE) {
			p.putPiece(color, ROOK, F1);
			p.updateZobrist(color, ROOK, F1);
		}
		else {
			p.putPiece(color, ROOK, F8);
			p.updateZobrist(color, ROOK, F8);
		}

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(p.getHalfMove() + 1);
		
		p.updateZobrist(color, p.getCastle(color));
		p.setCastle(color, Castle::NONE);
		p.updateZobrist(color, Castle::NONE);
		
		p.updateZobrist(p.getTurn());
		p.setTurn(Color(!color));
		p.updateZobrist(p.getTurn());

		break; }
	case EN_PASSANT:
		p.removePiece(color, piece, squareFrom);
		p.updateZobrist(color, piece, squareFrom);

		if (color == WHITE) {
			p.removePiece(Color(!color), PAWN, Square(squareTo - 8));
			p.updateZobrist(Color(!color), PAWN, Square(squareTo - 8));
		}
		else {
			p.removePiece(Color(!color), PAWN, Square(squareTo + 8));
			p.updateZobrist(Color(!color), PAWN, Square(squareTo + 8));
		}

		p.putPiece(color, piece, squareTo);
		p.updateZobrist(color, piece, squareTo);

		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() + 1);

		p.setHalfMove(0);

		p.updateZobrist(p.getEnPassant());
		p.setEnPassant(SQ_EMPTY);
		p.updateZobrist(SQ_EMPTY);

		p.updateZobrist(p.getTurn());
		p.setTurn(Color(!color));
		p.updateZobrist(p.getTurn());

		break;
	}

	if (!(moveType == EN_PASSANT) && !(p.getEnPassant() == SQ_EMPTY)) {
		Color temp;
		(p.getEnPassant() / 8 == RANK_3) ? temp = WHITE : temp = BLACK;
		
		if (!(temp == color)) {
			p.updateZobrist(p.getEnPassant());
			p.setEnPassant(SQ_EMPTY);
			p.updateZobrist(SQ_EMPTY);
		}

	}

	// if King moves, no more castle
	if (!(moveType == CASTLE_Q) && !(moveType == CASTLE_K) && (piece == KING)) {
		p.updateZobrist(color, p.getCastle(color));
		p.setCastle(color, Castle::NONE);
		p.updateZobrist(color, Castle::NONE);
	}

	// if Rook moves, no more castle on that side
	switch (color)
	{
	case WHITE:
		if (!(moveType == CASTLE_K) && piece == ROOK && squareFrom == H1
			&& (p.getCastle(WHITE) == Castle::KINGSIDE || p.getCastle(WHITE) == Castle::ALL)) {
			p.updateZobrist(WHITE, p.getCastle(WHITE));
			p.setCastle(WHITE, Castle(static_cast<ushort>(p.getCastle(WHITE)) - static_cast<ushort>(Castle::KINGSIDE)));
			p.updateZobrist(WHITE, p.getCastle(WHITE));
		}

		if (!(moveType == CASTLE_Q) && piece == ROOK && squareFrom == A1
			&& (p.getCastle(WHITE) == Castle::QUEENSIDE || p.getCastle(WHITE) == Castle::ALL)) {
			p.updateZobrist(WHITE, p.getCastle(WHITE));
			p.setCastle(WHITE, Castle(static_cast<ushort>(p.getCastle(WHITE)) - static_cast<ushort>(Castle::QUEENSIDE)));
			p.updateZobrist(WHITE, p.getCastle(WHITE));
		}

		break;
	case BLACK:
		if (!(moveType == CASTLE_K) && piece == ROOK && squareFrom == H8
			&& (p.getCastle(BLACK) == Castle::KINGSIDE || p.getCastle(BLACK) == Castle::ALL)) {
			p.updateZobrist(BLACK, p.getCastle(BLACK));
			p.setCastle(BLACK, Castle(static_cast<ushort>(p.getCastle(BLACK)) - static_cast<ushort>(Castle::KINGSIDE)));
			p.updateZobrist(BLACK, p.getCastle(BLACK));
		}

		if (!(moveType == CASTLE_Q) && piece == ROOK && squareFrom == A8
			&& (p.getCastle(BLACK) == Castle::QUEENSIDE || p.getCastle(BLACK) == Castle::ALL)) {
			p.updateZobrist(BLACK, p.getCastle(BLACK));
			p.setCastle(BLACK, Castle(static_cast<ushort>(p.getCastle(BLACK)) - static_cast<ushort>(Castle::QUEENSIDE)));
			p.updateZobrist(BLACK, p.getCastle(BLACK));
		}

		break;
	}

	// if Rook gets captured, no more castle, if previously possible
	if (color == BLACK && captured == ROOK && squareTo == H1
		&& (p.getCastle(WHITE) == Castle::KINGSIDE || p.getCastle(WHITE) == Castle::ALL)) {
		p.updateZobrist(WHITE, p.getCastle(WHITE));
		p.setCastle(WHITE, Castle(static_cast<ushort>(p.getCastle(WHITE)) - static_cast<ushort>(Castle::KINGSIDE)));
		p.updateZobrist(WHITE, p.getCastle(WHITE));
	}
	
	else if (color == BLACK && captured == ROOK && squareTo == A1
		&& (p.getCastle(WHITE) == Castle::QUEENSIDE || p.getCastle(WHITE) == Castle::ALL)) {
		p.updateZobrist(WHITE, p.getCastle(WHITE));
		p.setCastle(WHITE, Castle(static_cast<ushort>(p.getCastle(WHITE)) - static_cast<ushort>(Castle::QUEENSIDE)));
		p.updateZobrist(WHITE, p.getCastle(WHITE));
	}
	
	else if (color == WHITE && captured == ROOK && squareTo == H8
		&& (p.getCastle(BLACK) == Castle::KINGSIDE || p.getCastle(BLACK) == Castle::ALL)) {
		p.updateZobrist(BLACK, p.getCastle(BLACK));
		p.setCastle(BLACK, Castle(static_cast<ushort>(p.getCastle(BLACK)) - static_cast<ushort>(Castle::KINGSIDE)));
		p.updateZobrist(BLACK, p.getCastle(BLACK));
	}
	
	else if (color == WHITE && captured == ROOK && squareTo == A8
		&& (p.getCastle(BLACK) == Castle::QUEENSIDE || p.getCastle(BLACK) == Castle::ALL)) {
		p.updateZobrist(BLACK, p.getCastle(BLACK));
		p.setCastle(BLACK, Castle(static_cast<ushort>(p.getCastle(BLACK)) - static_cast<ushort>(Castle::QUEENSIDE)));
		p.updateZobrist(BLACK, p.getCastle(BLACK));
	}
}


void undoMove(Move const &m, Position &p, Position const &backup)
{
	p = backup;
}