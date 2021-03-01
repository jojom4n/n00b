#include "pch.h"
#include "Position.h"
#include "attack.h"
#include "bitscan.h"
#include "overloading.h"

extern struct LookupTable g_MoveTables; // see attack.cpp (and its header file)

Position::Position()
{

}


Position::~Position()
{
	
}


void Position::setNew()
{	
	// white pieces
	putPiece(WHITE, KING, E1);
	putPiece(WHITE, QUEEN, D1);
	putPiece(WHITE, ROOK, A1);
	putPiece(WHITE, ROOK, H1);
	putPiece(WHITE, KNIGHT, B1);
	putPiece(WHITE, KNIGHT, G1);
	putPiece(WHITE, BISHOP, C1);
	putPiece(WHITE, BISHOP, F1);
	
	for (Square i = A2; i <= H2; i++)
		putPiece(WHITE, PAWN, i);

	//black pieces
	putPiece(BLACK, KING, E8);
	putPiece(BLACK, QUEEN, D8);
	putPiece(BLACK, ROOK, A8);
	putPiece(BLACK, ROOK, H8);
	putPiece(BLACK, KNIGHT, B8);
	putPiece(BLACK, KNIGHT, G8);
	putPiece(BLACK, BISHOP, C8);
	putPiece(BLACK, BISHOP, F8);

	for (Square i = A7; i <= H7; i++)
		putPiece(BLACK, PAWN, i);

	enPassantSquare_ = SQ_EMPTY;
	turn_ = WHITE;
	moveNumber_ = 1;
	halfMove_ = 0;
	castle_[WHITE] = Castle::ALL;
	castle_[BLACK] = Castle::ALL;

	setZobrist();
}


void Position::resetPosition()
{
	board_.fill({});
	whitePieces_ = C64(0), blackPieces_ = C64(0), allPieces_ = C64(0);
	zobristHash_ = 0;
}


void Position::storeState(ushort const &depth)
{
	prvState[depth].prvTurn_ = turn_;
	prvState[depth].prvMoveNumber_ = moveNumber_;
	prvState[depth].prvHalfMove = halfMove_;
	prvState[depth].prvEnPassantSquare_ = enPassantSquare_;
	prvState[depth].prvCastle_ = castle_;
	prvState[depth].prvZobristHash_ = zobristHash_;
}


void Position::restoreState(ushort const &depth)
{
	turn_ = prvState[depth].prvTurn_;
	moveNumber_ = prvState[depth].prvMoveNumber_;
	halfMove_ = prvState[depth].prvHalfMove;
	enPassantSquare_ = prvState[depth].prvEnPassantSquare_;
	castle_ = prvState[depth].prvCastle_;
	zobristHash_ = prvState[depth].prvZobristHash_;
}

void Position::putPiece(Color const &color, Piece const &piece, Square const &square)
{
	board_[color][piece] |= C64(1) << square;
	color == WHITE ? whitePieces_ |= C64(1) << square : blackPieces_ |= C64(1) << square;
	allPieces_ |= C64(1) << square;
}


void Position::removePiece(Color const &color, Piece const &piece, Square const &square)
{
	board_[color][piece] &= ~(C64(1) << square);
	color == WHITE ? whitePieces_ &= ~(C64(1) << square) : blackPieces_ &= ~(C64(1) << square);
	allPieces_ &= ~(C64(1) << square);
}


const PieceID Position::idPiece(Square const &square, Color const &color) const
{
	/* let us AND the bit set in the square (1ULL << square) with the main bitboards,
	until we find the one containing that bit */

	if (!(color == ALL_COLOR)) {
		if (board_[color][PAWN] & (C64(1) << square))
			return PieceID{ color, PAWN };
		else if (board_[color][KNIGHT] & (C64(1) << square))
			return PieceID{ color, KNIGHT };
		else if (board_[color][BISHOP] & (C64(1) << square))
			return PieceID{ color, BISHOP };
		else if (board_[color][ROOK] & (C64(1) << square))
			return PieceID{ color, ROOK };
		else if (board_[color][QUEEN] & (C64(1) << square))
			return PieceID{ color, QUEEN };
		else if (board_[color][KING] & (C64(1) << square))
			return PieceID{ color, KING };
	}
	else {
		for (Color x = BLACK; x <= WHITE; x++)
			for (Piece y = KING; y <= PAWN; y++) {
				if (board_[x][y] & (C64(1) << square))
					return PieceID{ x, y };
			}
	}
	
	return { ALL_COLOR, NO_PIECE };  // no piece on square
}


// see https://www.chessprogramming.org/Population_Count
const ushort Position::count(Color const &color) const 
{
	switch (color) {
	case WHITE:
		return ushort(popcount(&whitePieces_));
		break;
	case BLACK:
		return ushort(popcount(&blackPieces_));
		break;
	default:
		return ushort(popcount(&blackPieces_));
		break;
	}
}


const ushort Position::countPieceType(Color const &color, Piece const &piece) const
{
	return ushort(popcount(&board_[color][piece]));
}


const bool Position::isEnding() const
{
	ushort count{};

	for (Color c = BLACK; c <= WHITE; c++) {
		count += countPieceType(c, QUEEN);
		count += countPieceType(c, BISHOP);
		count += countPieceType(c, ROOK);
	}

	return count > 0 ? false : true;
}


const std::vector<Square> Position::getSquareOfPiece(Color const &color, Piece const &piece) const
{
	std::vector<Square> squares;
	Bitboard temp = board_[color][piece];

	while (temp) 
		 squares.push_back(Square(bitscan_reset(temp)));
	
	return squares;
}

const Square Position::getKingSquare(Color const &color) const
{
	return Square(bitscan_fwd(board_[color][KING]));
}


bool Position::isSquareAttacked(Square const &square) const
{
	Bitboard mask;
	Color enemyColor;

	if (whitePieces_ & (C64(1) << square))
		enemyColor = BLACK;
	else if (blackPieces_ & (C64(1) << square))
		enemyColor = WHITE;
	else
		enemyColor = Color(!turn_); // no b/w on square, we are computing for castle, so let's see if opponent attacks it

	// ROOK
	mask = g_MoveTables.rook(square, allPieces_); // does rook's attack mask...
	if (mask &= board_[enemyColor][ROOK]) // ...intersect the square?
		return true;

	// BISHOP
	mask = g_MoveTables.bishop(square, allPieces_); // does bishop's attack mask...
	if (mask &= board_[enemyColor][BISHOP]) // ...intersect square?
		return true;

	// QUEEN
	mask = g_MoveTables.rook(square, allPieces_) | g_MoveTables.bishop(square, allPieces_); // does queen's attack mask...
	if (mask &= board_[enemyColor][QUEEN]) // ...intersect square?
		return true;

	// KNIGHT
	mask = g_MoveTables.knight[square]; // does knight's attack mask...
	if (mask &= board_[enemyColor][KNIGHT]) // ...intersect square?
		return true;

	// KING
	mask = g_MoveTables.king[square]; // does enemy king's attack mask...
	if (mask &= board_[enemyColor][KING]) // ...intersect square?
		return true;

	//PAWNS
	// does enemy pawn's attack mask...
	(enemyColor == WHITE) ? mask = g_MoveTables.blackPawn[square]
		: mask = g_MoveTables.whitePawn[square];

	if (mask &= board_[enemyColor][PAWN]) // ...intersect square?
		return true;

	return false; // if all above fails, then square is not attacked
}
