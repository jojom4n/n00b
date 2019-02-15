#include "pch.h"
#include "protos.h"
#include "Position.h"
#include <sstream>

bool fenValidate(const std::stringstream &fen)
{
	return true;
}

void fenParser(std::stringstream &fen, Position &board)
{
	std::vector<std::string> buffer {};
	std::string word;
	
	// "rewind" the input stream (i.e. clear its EOF state and rewind to start position)
	fen.clear();
	fen.seekg(0);
	
	while (fen >> word)
		buffer.push_back(word); // extract from the input stream to a string vector

	
	// * * * BOARD POSITION FROM FEN * * *
	board.resetPosition(); // reset the bitboards in board to empty

	ushort f = FILE_A; // file initialization
	char c = ' '; // char initialization: we'll read the FEN into this

	for (ushort i = 0, r = RANK_8; i < buffer[2].length(); i++) { // loop until end of FEN string
		c = buffer[2][i]; // read a character from FEN string
		if (c != '/') // is the end of the row in FEN string?
			switch (c) { // if not, compute the character
			case 'R':
				board.putPiece(WHITE, ROOKS, Square((8 * r) + f));
				f++;
				break;
			case 'r':
				board.putPiece(BLACK, ROOKS, Square((8 * r) + f));
				f++;
				break;
			case 'N':
				board.putPiece(WHITE, KNIGHTS, Square((8 * r) + f));
				f++;
				break;
			case 'n':
				board.putPiece(BLACK, KNIGHTS, Square((8 * r) + f));
				f++;
				break;
			case 'B':
				board.putPiece(WHITE, BISHOPS, Square((8 * r) + f));
				f++;
				break;
			case 'b':
				board.putPiece(BLACK, BISHOPS, Square((8 * r) + f));
				f++;
				break;
			case 'P':
				board.putPiece(WHITE, PAWNS, Square((8 * r) + f));
				f++;
				break;
			case 'p':
				board.putPiece(BLACK, PAWNS, Square((8 * r) + f));
				f++;
				break;
			case 'Q':
				board.putPiece(WHITE, QUEEN, Square((8 * r) + f));
				f++;
				break;
			case 'q':
				board.putPiece(BLACK, QUEEN, Square((8 * r) + f));
				f++;
				break;
			case 'K':
				board.putPiece(WHITE, KING, Square((8 * r) + f));
				f++;
				break;
			case 'k':
				board.putPiece(BLACK, KING, Square((8 * r) + f));
				f++;
				break;
			case '1': // if character is a number, then jump corresponding files
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				f += c - '0';
				break;
			default:
				break;
			}
		else
			r--, f = FILE_A; // if c = '/', then move to a new row and start again to parse from file A
	}


	// * * * OTHER BOARD PARAMETERS FROM FEN * * *
	(buffer[3] == "w") ? board.setTurn(WHITE) : board.setTurn(BLACK); // who can move, black or white?
	
	//Castling rights. Extract from FEN, then write to the board
	ushort castle_white{}, castle_black{};
	
	for (ushort i = 0; i < buffer[4].length(); i++) {
		c = buffer[4][i];
		switch (c) {
		case 'K':
			castle_white += 1;
			break;
		case 'k':
			castle_black += 1;
			break;
		case 'Q':
			castle_white += 2;
			break;
		case 'q':
			castle_black += 2;
			break;
		default:
			break;
		}
	}
	
	board.setCastle(WHITE, castle_white);
	board.setCastle(BLACK, castle_black);

	// En passant square
	std::transform(buffer[5].begin(), buffer[5].end(), buffer[5].begin(), ::tolower); // lower-case the en-passant value, just in case
	if (StringSquareMap.find(buffer[5]) != StringSquareMap.end()) // is there a valid value, corresponding to map?
		board.setEnPassant(StringSquareMap[buffer[5]]);
	else
		board.setEnPassant(SQ_EMPTY);

	board.setHalfMove(std::stoi(buffer[6], nullptr, 0)); // set half move

	board.setMoveNumber(std::stoi(buffer[7], nullptr, 0)); // set move number
	   	 
	display_board(board);
}
