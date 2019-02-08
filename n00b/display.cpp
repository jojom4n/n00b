#include "pch.h"
#include <string>
#include "Board.h"
#include "protos.h"

const std::string squares_to_string[65] =
{
	"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
	"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
	"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
	"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
	"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
	"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
	"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
	"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8",
	"NONE"
};

const std::string display_board(Board const &board)
{
	std::string output = "+---+---+---+---+---+---+---+---+\n";

	ushort i = 1;

	/* Need to explain what we are going to do in the following for...loop.

		We will reverse scan the all_pieces bitboard rank by rank, that is, from 8th rank 
		to 1st one. For each rank,	we will scan from file A to file H and we will print 
		an "X" for every piece we will find in the bitboard.

		Therefore, the scan will proceed as follows: square 56 (A8), square 57 (B8), square 58 (C8)
		...square 63(H8), and then square 48 (A7), square 49 (B7), square 50 (C7)...square 55 (H7),
		etc.

		Now, to lookup the squares in the above order, I found on my own the following formula:

		SquareToScan = (rank ^ 2) + [(rank * i) + file]

		where 'i' is the index we have declared just before the for...loop statement, going to be
		incremented by 1 for every rank we will go through.

		I know that the above formula is far from being elegant or efficient, but it works.
		Anyway, I could not find a better formula somewhere.
		*/

	for (short int rank = RANK_8; rank >= RANK_1; rank--)	// cannot use ushort: by definition
															// ushort is always >= 0
	{
		
		for (ushort file = FILE_A; file <= FILE_H; file++) {

			Square check_square = Square(rank * rank + ((rank * i) + file));
			
			if (board.is_square_occupied(check_square)) // square not empty
			{
				output += "| ";
				output += print_piece(board.identify_piece(check_square));
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

const char print_piece (bb_index const &coordinates)
{
	switch (coordinates.y)
	{
	case king:
		if (coordinates.x == white)
			return 'K';
		else
			return 'k';
		break;
	
	case queen:
		if (coordinates.x == white)
			return 'Q';
		else
			return 'q';
		break;

	case rooks:
		if (coordinates.x == white)
			return 'R';
		else
			return 'r';
		break;

	case knights:
		if (coordinates.x == white)
			return 'N';
		else
			return 'n';
		break;

	case bishops:
		if (coordinates.x == white)
			return 'B';
		else
			return 'b';
		break;

	case pawns:
		if (coordinates.x == white)
			return 'P';
		else
			return 'p';
		break;
	default:
		return NULL; // some error occurred
		break;
	}
}
