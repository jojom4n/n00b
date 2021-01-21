#include "pch.h"
#include "Position.h"
#include "attack.h"
#include "bitscan.h"
#include "overloading.h"
#include "tt.h"

extern struct LookupTable g_MoveTables; // see attack.cpp (and its header file)
extern std::array<TTEntry, TT_SIZE> TT::table;


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
	
	TT::table.fill({}); // clear TT
}


void Position::resetPosition()
{
	board_.fill({});
	whitePieces_ = C64(0), blackPieces_ = C64(0), allPieces_ = C64(0);
	zobristHash_ = 0;
}


void Position::putPiece(Color const &color, Piece const &piece, Square const &square)
{
	board_[color][piece] |= C64(1) << square;
	update(color);
}


void Position::removePiece(Color const &color, Piece const &piece, Square const &square)
{
	board_[color][piece] &= ~(C64(1) << square);
	update(color);
}


void Position::update(Color const& color)
{
	if (color == WHITE) {
		whitePieces_ = 0;
		for (ushort i = 0; i < 6; i++)
			whitePieces_ |= board_[WHITE][i];
	}
	else if (color == BLACK) {
		blackPieces_ = 0;
		for (ushort i = 0; i < 6; i++)
			blackPieces_ |= board_[BLACK][i];
	}
		
	allPieces_ = whitePieces_ | blackPieces_;
}


const PieceID Position::idPiece(Square const &square, Color const &color) const
{
	/* let us AND the bit set in the square (1ULL << square) with the main bitboards,
	until we find the one containing that bit */
	switch (color)
	{
	case WHITE:
		for (ushort y = 0; y < 6; y++)
			if (board_[WHITE][y] & (C64(1) << square)) {
				ushort x = WHITE;
				return PieceID{ (Color)x, (Piece)y };
			}
		break;
	case BLACK:
		for (ushort y = 0; y < 6; y++)
			if (board_[BLACK][y] & (C64(1) << square)) {
				ushort x = BLACK;
				return PieceID{ (Color)x, (Piece)y };
			}
		break;
	case ALL_COLOR:
		for (ushort x = 0; x < 2; x++)
			for (ushort y = 0; y < 6; y++) {
				if (board_[x][y] & (C64(1) << square))
					return PieceID{ (Color)x, (Piece)y };
			}
		break;
	}
	
	return { ALL_COLOR, NO_PIECE };  // no piece on square
}


// see https://www.chessprogramming.org/Population_Count
const ushort Position::count(Color const &color) const 
{
	switch (color) {
	case WHITE:
		return ushort(popcnt(&whitePieces_, sizeof(whitePieces_)));
		break;
	case BLACK:
		return ushort(popcnt(&blackPieces_, sizeof(blackPieces_)));
		break;
	default:
		return ushort(popcnt(&blackPieces_, sizeof(blackPieces_)));
		break;
	}
}


const ushort Position::countPieceType(Color const &color, Piece const &piece) const
{
	return ushort(popcnt(&board_[color][piece], sizeof(board_[color][piece])));
}


const bool Position::isEnding() const
{
	ushort count{};

	for (Color c = BLACK; c <= WHITE; c++)
		for (Piece piece = QUEEN; piece <= BISHOP; piece++)
			count += countPieceType(c, piece);

	if (count > 0)
		return false;
	else
		return true;
}


const std::vector<Square> Position::getPieceOnSquare(Color const &color, Piece const &piece) const
{
	std::vector<Square> squares;
	Bitboard temp = board_[color][piece];

	while (temp > 0) 
		 squares.push_back(Square(bitscan_reset(temp)));
	
	return squares;
}

const bool Position::isSquareAttackedBy(Color const &color, Square const &square) const
{
	Bitboard sq{}, occ = getPosition(), enemy{}, mask{};
	
	sq |= C64(1) << square;

	// ROOK
	enemy = getPieces(color, ROOK); // get rook's bitboard
	mask = g_MoveTables.rook(square, occ); // does rook's attack mask...
	if (mask &= enemy) // ...intersect the square?
		return true;

	// BISHOP
	enemy = getPieces(color, BISHOP); // get bishop's bitboard
	mask = g_MoveTables.bishop(square, occ); // does bishop's attack mask...
	if (mask &= enemy) // ...intersect square?
		return true;

	// QUEEN
	enemy = getPieces(color, QUEEN); // get queen's bitboard
	mask = g_MoveTables.rook(square, occ) | g_MoveTables.bishop(square, occ); // does queen's attack mask...
	if (mask &= enemy) // ...intersect square?
		return true;

	// KNIGHT
	enemy = getPieces(color, KNIGHT); // get knight's bitboard
	mask = g_MoveTables.knight[square]; // does knight's attack mask...
	if (mask &= enemy) // ...intersect square?
		return true;

	// KING
	enemy = getPieces(color, KING); // get enemy king's bitboard
	mask = g_MoveTables.king[square]; // does enemy king's attack mask...
	if (mask &= enemy) // ...intersect square?
		return true;

	//PAWNS
	enemy = getPieces(color, PAWN); // get pawn's bitboard
	// does enemy pawn's attack mask...
	(color == WHITE) ? mask = g_MoveTables.whitePawn(enemy, sq) : mask = g_MoveTables.blackPawn(enemy, sq);
	if (mask &= sq) // ...intersect square?
		return true;
	
	return false; // if all above fails, then square is not attacked
}