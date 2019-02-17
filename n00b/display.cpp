#include "pch.h"
#include <string>
#include <iostream>
#include "Position.h"
#include "protos.h"

extern std::map<std::string, Square> StringToSquareMap = {
	{ "a1",A1 }, { "b1",B1 }, { "c1",C1 }, { "d1",D1 }, { "e1",E1 }, { "f1",F1 }, { "g1",G1 }, { "h1",H1 },
	{ "a2",A2 }, { "b2",B2 }, { "c2",C2 }, { "d2",D2 }, { "e2",E2 }, { "f2",F2 }, { "g2",G2 }, { "h2",H2 },
	{ "a3",A3 }, { "b3",A3 }, { "c3",C3 }, { "d3",D3 }, { "e3",E3 }, { "f3",F3 }, { "g3",G3 }, { "h3",H3 },
	{ "a4",A4 }, { "b4",B4 }, { "c4",C4 }, { "d4",D4 }, { "e4",E4 }, { "f4",F4 }, { "g4",G4 }, { "h4",H4 },
	{ "a5",A5 }, { "b5",B5 }, { "c5",C5 }, { "d5",D5 }, { "e5",E5 }, { "f5",F5 }, { "g5",G5 }, { "h5",H5 },
	{ "a6",A6 }, { "b6",B6 }, { "c6",C6 }, { "d6",D6 }, { "e6",E6 }, { "f6",F6 }, { "g6",G6 }, { "h6",H6 },
	{ "a7",A7 }, { "b7",B7 }, { "c7",C7 }, { "d7",D7 }, { "e7",E7 }, { "f7",F7 }, { "g7",G7 }, { "h7",H7 },
	{ "a8",A8 }, { "b8",B8 }, { "c8",C8 }, { "d8",D8 }, { "e8",E8 }, { "f8",F8 }, { "g8",G8 }, { "h8",H8 },
	{ "-",SQ_EMPTY }
};

extern std::map<Square, std::string> SquareToStringMap = {
	{ A1,"a1" }, { B1,"b1" }, { C1,"c1" }, { D1,"d1"}, {E1,"e1" }, { F1,"f1"}, {G1,"g1"},{H1,"h1"},
	{ A2,"a2" }, { B2,"b2" }, { C2,"c2" }, { D2,"d2"}, {E2,"e2" }, { F2,"f2"}, {G2,"g2"},{H2,"h2"},
	{ A3,"a3" }, { B3,"b3" }, { C3,"c3" }, { D3,"d3"}, {E3,"e3" }, { F3,"f3"}, {G3,"g3"},{H3,"h3"},
	{ A4,"a4" }, { B4,"b4" }, { C4,"c4" }, { D4,"d4"}, {E4,"e4" }, { F4,"f4"}, {G4,"g4"},{H4,"h4"},
	{ A5,"a5" }, { B5,"b5" }, { C5,"c5" }, { D5,"d5"}, {E5,"e5" }, { F5,"f5"}, {G5,"g5"},{H5,"h5"},
	{ A6,"a6" }, { B6,"b6" }, { C6,"c6" }, { D6,"d6"}, {E6,"e6" }, { F6,"f6"}, {G6,"g6"},{H6,"h6"},
	{ A7,"a7" }, { B7,"b7" }, { C7,"c7" }, { D7,"d7"}, {E7,"e7" }, { F7,"f7"}, {G7,"g7"},{H7,"h7"},
	{ A8,"a8" }, { B8,"b8" }, { C8,"c8" }, { D8,"d8"}, {E8,"e8" }, { F8,"f8"}, {G8,"g8"},{H8,"h8"},
	{ SQ_EMPTY,"-" }
};

std::map<ushort, std::string> CastleMap = {
	{ KINGSIDE,"K-" }, { QUEENSIDE,"-Q" }, { ALL,"KQ" }, { NONE,"--" }
};


void display_board(Position const &board)
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
	
	std::cout << std::endl << output << std::endl;
	
	(board.getTurn() == WHITE) ? std::cout << "\nSide to move: White\t" : std::cout << "Side to move: Black\t";
	
	std::string castleDisplayBlack = CastleMap[board.getCastle(BLACK)];
	std::transform(castleDisplayBlack.begin(), castleDisplayBlack.end(), castleDisplayBlack.begin(), ::tolower);
	std::cout << "Castling rights: " << CastleMap[board.getCastle(WHITE)] << castleDisplayBlack << std::endl;
	std::cout << "En-passant square: " << SquareToStringMap[Square(board.getEnPassant())] << "\t";
	std::cout << "Half-move: " << board.getHalfMove() << "\tMove number: " << board.getMoveNumber();
	std::cout << std::endl;
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
