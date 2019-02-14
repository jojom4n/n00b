#include "pch.h"
#include <string>
#include "Position.h"
#include "protos.h"

const std::array<std::string, 65> squares_to_string =
{
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
	"NONE"
};


const std::string display_board(Position const &board)
{
	std::string output = "+---+---+---+---+---+---+---+---+\n";

	ushort i = 1;

	/* Need to explain what we are going to do in the following for...loop.

		We will reverse scan the allPieces_ bitboard rank by rank, that is, from 8th rank 
		to 1st one. For each rank,	we will scan from file A to file H and we will print 
		an "X" for every piece we will find in the bitboard.

		Therefore, the scan will proceed as follows: square 56 (A8), square 57 (B8), square 58 (C8)
		...square 63(H8), and then square 48 (A7), square 49 (B7), square 50 (C7)...square 55 (H7),
		etc.

		Now, to lookup the squares in the above order, I found on my own the following formula:

		SquareToScan = (rank ^ 2) + [(rank * i) + file]

		where 'i' is the index we have declared just before the for...loop statement, going to be
		incremented by 1 for every rank we will go through	*/

	for (short rank = RANK_8; rank >= RANK_1; rank--)	// cannot use ushort: by definition
															// ushort is always >= 0
	{
		
		for (ushort file = FILE_A; file <= FILE_H; file++) {

			Square square = Square(rank * rank + ((rank * i) + file));
			
			if (board.occupiedSquare(square)) // square not empty
			{
				output += "| ";
				output += printPiece(board.idPiece(square));
				output += " ";
			}
			
			else
				output += "|   ";
		}
		
		output += "| ";
		output += rank + 49;
		output += "\n+---+---+---+---+---+---+---+---+\n";
		i++;
	}
	
	output += "  A   B   C   D   E   F   G   H\n";
	
	return output;
}


const char printPiece (coords const &coordinates)
{
	switch (coordinates.y)
	{
	case KING:
		return (coordinates.x == WHITE) ? 'K' : 'k';
		break;
	case QUEEN:
		return (coordinates.x == WHITE) ? 'Q' : 'q';
		break;
	case ROOKS:
		return (coordinates.x == WHITE) ? 'R' : 'r';
		break;
	case KNIGHTS:
		return (coordinates.x == WHITE) ? 'N' : 'n';
		break;
	case BISHOPS:
		return (coordinates.x == WHITE) ? 'B' : 'b';
		break;
	case PAWNS:
		return (coordinates.x == WHITE) ? 'P' : 'p';
		break;
	default:
		return NULL; // some error occurred
		break;
	}
}
