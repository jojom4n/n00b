#include "pch.h"
#include "bitscan.h"
#include "nnue/nnue.h"
#include "nnue_eval.h"
#include "overloading.h"
#include <map>
#include <utility>


// to use NNUE-probe we must map our piece notation to Stockfish's one via the following map
std::map<std::pair<const int, const int> const, int> NNUEmap = {
	{ std::make_pair(WHITE, KING), 1 },   { std::make_pair(WHITE, QUEEN), 2 },   { std::make_pair(WHITE, ROOK), 3 },    { std::make_pair(WHITE, BISHOP), 4 },
	{ std::make_pair(WHITE, KNIGHT), 5 }, { std::make_pair(WHITE, PAWN), 6 },    { std::make_pair(BLACK, KING), 7 },    { std::make_pair(BLACK, QUEEN), 8 },
	{ std::make_pair(BLACK, ROOK), 9 },   { std::make_pair(BLACK, BISHOP), 10 }, { std::make_pair(BLACK, KNIGHT), 11 }, { std::make_pair(BLACK, PAWN), 12 }
};


void init_NNUE(const char* filename)
{
	nnue_init(filename);
}


int evalFEN_NNUE(const char* fen)
{
	return nnue_evaluate_fen(fen);
}


int eval_NNUE(Position const& p)
{
	int nnue_pieces[33]{}, nnue_squares[33]{};
	fill_NNUE(p, nnue_pieces, nnue_squares);
	int score = nnue_evaluate(p.getTurn(), nnue_pieces, nnue_squares);
	return score;
}


void fill_NNUE(Position const& p, int* nnue_pieces, int* nnue_squares)
{
	ushort index = 2; // in NNUE, piece[0] and piece[1] must be kings, so index starts from 2
	
	nnue_squares[0] = p.getKingSquare(WHITE);
	nnue_pieces[0] = NNUEmap[ {WHITE, KING} ];
	nnue_squares[1] = p.getKingSquare(BLACK);
	nnue_pieces[1] = NNUEmap[ {BLACK, KING} ];

	for (short color = WHITE; color >= BLACK; color--) {
		for (Piece piece = QUEEN; piece <= PAWN; piece++) {
			Bitboard bb = p.getPieces(Color(color), piece);
			while (bb) {
				Square sq = static_cast<Square>(bitscan_reset(bb));
				nnue_squares[index] = sq;
				nnue_pieces[index] = NNUEmap[{Color(color), piece}];
				index++;
			}
		}
	}
	
	 	
	// set zero at the end of pieces and squares arrays
	nnue_pieces[index] = 0;
}