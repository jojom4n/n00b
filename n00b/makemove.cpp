#include "pch.h"
#include "Position.h"


void doMove(Move const &m, Position &p)
{
	Square squareFrom{}, squareTo{};
	ushort moveType = ((C64(1) << 3) - 1) & (m >> 7);
	squareFrom = Square(((C64(1) << 6) - 1) & (m >> 20));
	squareTo = Square(((C64(1) << 6) - 1) & (m >> 14));
	Color color = Color(((C64(1) << 1) - 1) & (m >> 13));
	Piece piece = Piece(((C64(1) << 3) - 1) & (m >> 10));
	Piece captured = Piece(((C64(1) << 3) - 1) & (m >> 4));
	ushort promotedTo = ((C64(1) << 3) - 1) & (m >> 1);
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
			

		(promotedTo == PAWN_TO_KNIGHT) ? p.putPiece(color, KNIGHT, squareTo) : p.putPiece(color, QUEEN, squareTo);

		p.setTurn(Color(color));
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

	if (!(moveType == EN_PASSANT) && !(p.getEnPassant() == SQ_EMPTY))
		p.setEnPassant(SQ_EMPTY);

	// if King moves, no more castle
	if (!(moveType == CASTLE_Q) && !(moveType == CASTLE_K) && (piece == KING))
		p.setCastle(color, NONE);

	// if Rook moves, no more castle on that side
	switch (color)
	{
	case WHITE:
		if (!(moveType == CASTLE_Q) && (piece == ROOK) && (squareFrom == A1))
			p.setCastle(WHITE, p.getCastle(WHITE) - 2);
		if (!(moveType == CASTLE_K) && (piece == ROOK) && (squareFrom == H1))
			p.setCastle(WHITE, p.getCastle(WHITE) - 1);
		break;
	case BLACK:
		if (!(moveType == CASTLE_Q) && (piece == ROOK) && (squareFrom == A8))
			p.setCastle(BLACK, p.getCastle(BLACK) - 2);
		if (!(moveType == CASTLE_K) && (piece == ROOK) && (squareFrom == H8))
			p.setCastle(BLACK, p.getCastle(BLACK) - 1);
		break;
	}
}


void undoMove(Move const &m, Position &p, Position const &backup)
{
	/* Square squareFrom{}, squareTo{};
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

		p.setHalfMove(p.getBackupHalfMove());
		p.setTurn(Color(!p.getTurn()));
		break;
	case PROMOTION:
		(promotedTo == PAWN_TO_KNIGHT) ? p.removePiece(color, KNIGHT, squareTo) : p.removePiece(color, QUEEN, squareTo);
		
		if (!(captured == NO_PIECE)) {
			p.putPiece(color, captured, squareTo);
			p.setHalfMove(p.getBackupHalfMove());
		}
		else {
			p.putPiece(color, PAWN, squareFrom);
			p.setHalfMove(p.getHalfMove() - 1);
		}
		
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
		(color == WHITE) ? p.putPiece(Color(!color), PAWN, Square(squareTo -8))
				: p.putPiece(Color(!color), PAWN, Square(squareTo + 8));
		
		p.setHalfMove(p.getBackupHalfMove());
		
		if (color == BLACK)
			p.setMoveNumber(p.getMoveNumber() - 1);

		// p.setEnPassant(p.getBackupEP());
		p.setTurn(Color(!p.getTurn()));
		break;
	}

		p.setEnPassant(p.getBackupEP()); */

	p = backup;
}