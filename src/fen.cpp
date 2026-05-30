#include "pch.h"
#include "fen.h"
#include "defs.h"
#include "display.h"
#include "enums.h"
#include "movegen.h"
#include "overloading.h"
#include "Position.h"
#include <algorithm>
#include <map>
#include <vector>
#include <sstream>


extern std::map<std::string, Square> stringToSquareMap; // see display.cpp

// Generate FEN string from Position
std::string positionToFEN(const Position& pos) {
    std::string fen = "";
    
    // Loop through each rank from 8 to 1
    for (Rank r = RANK_8; r >= RANK_1; r--) {
        ushort emptyCount = 0;
        
        // Loop through each file from A to H
        for (File f = FILE_A; f <= FILE_H; f++) {
            Square sq = Square((8 * r) + f);
            PieceID p = pos.idPiece(sq);
            
            if (p.piece == NO_PIECE) 
                emptyCount++;
            else {
                if (emptyCount) { 	// reset empty squares buffer 
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                fen += printPiece(p);
            }
        }
        
        if (emptyCount > 0) { 	// let's add numbers of eventual empty squares at the end of the files
            fen += std::to_string(emptyCount); 
        }
        
        if (r > RANK_1) { 	// no trailing character for rank 1
            fen += "/";
        }
    }
    
    // Side to move
    fen += " ";
    fen += (pos.getTurn() == WHITE) ? "w" : "b";
    
    // Castling rights
    fen += " ";
    bool hasCastle = false;
    if (pos.getCastle(WHITE) & KINGSIDE) { fen += "K"; hasCastle = true; }
    if (pos.getCastle(WHITE) & QUEENSIDE) { fen += "Q"; hasCastle = true; }
    if (pos.getCastle(BLACK) & KINGSIDE) { fen += "k"; hasCastle = true; }
    if (pos.getCastle(BLACK) & QUEENSIDE) { fen += "q"; hasCastle = true; }
    if (!hasCastle) { fen += "-"; }
    
    // En passant square
    fen += " ";
    Square sq = pos.getEnPassant();
    if (sq != SQ_EMPTY) {
        fen += std::string(1, 'a' + (sq % 8));
        fen += std::to_string(8 - (sq / 8));
    } else {
        fen += "-";
    }
    
    // Half move clock
    fen += " ";
    fen += std::to_string(pos.getHalfMove());
    
    // Full move number
    fen += " ";
    fen += std::to_string(pos.getMoveNumber());
    
    return fen;
}


extern std::map<std::string, Square> stringToSquareMap; // see display.cpp

const bool fenValidate(std::stringstream &fen)
{
	std::vector<std::string> buffer{};
	std::string word;

	// "rewind" the input stream (i.e. clear its EOF state and rewind to start position)
	fen.clear();
	fen.seekg(0);

	while (fen >> word)
		buffer.push_back(word); // extract from the input stream to a string vector
	
	if (buffer.size() < 6) // is FEN complete? (board turn castling enpassant halfmove fullmove)
		return false;
							
	/* Validate the position inside the FEN. To be valid, we must have 8 rows, 8 files and two kings.
	Besides, characters cannot be different from 0-8 and piece letters.
	rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 */
	ushort rows{}, files{};
	bool whiteKing{}, blackKing{};
	
	for (ushort i = 0; i < buffer[0].length(); i++)
	{
		char c = ' ';
		c = buffer[0][i];
		
		switch(c) {
			case 'K':
				whiteKing = true;
				files++;
				break;
			case 'k':
				blackKing = true;
				files++;
				break;
			case '/':
				rows++;
				if (!(files == 8)) 
					return false;
				else 
					files=0; // reset files every time we meet a '/'
				break;
			case 'r':
			case 'R':
			case 'n':
			case 'N':
			case 'b':
			case 'B':
			case 'q':
			case 'Q':
			case 'p':
			case 'P':
				files++;
				break;
			case '1': // if character is a number, then jump corresponding files
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				files += c - '0';
				break;
			default:
				return false;
				break;
		}
	}

	if (!(rows == 7) || !(whiteKing) || !(blackKing)) // 7 are the '/' in valid FEN
		return false;
	
	// Side to move. This part of FEN must be 'w' or 'b'
	if (!(buffer[1] == "w") && !(buffer[1]=="b"))
		return false;

	/* Castling rights from FEN. It must be '-' or some combination of 'K','Q','k','q',
	anyway maximum 4 characters */
	ushort countK{}, countk{}, countQ{}, countq{}; // avoid two 'K's or two 'Q's (black or white)

	if (buffer[2].length() > 4)
			return false;
	else {
		for (ushort i = 0; i < buffer[2].length(); i++) {
			char c = ' ';
			c = buffer[2][i];
			if (!(c == 'K') && !(c == 'k') && !(c == 'Q') && !(c == 'q') && !(c == '-'))
				return false;
			
			switch (c) {
			case 'K':
				countK++;
				break;
			case 'k':
				countk++;
				break;
			case 'Q':
				countQ++;
				break;
			case 'q':
				countq++;
				break;
			}

			if (countK > 1 || countk > 1 || countQ > 1 || countq > 1)
				return false;
		}
	}

	// now verify en-passant in FEN. It must be '-' or a square
	if (buffer.size() > 3) {
		if (!(buffer[3] == "-")) {
			if (buffer[3].length() != 2)
				return false;
			// Check if it's a valid square notation
			if (!((buffer[3][0] >= 'a' && buffer[3][0] <= 'h') &&
			      (buffer[3][1] >= '1' && buffer[3][1] <= '8')))
				return false;
		}
	}

	// if none of the above conditions occurs, then FEN is valid
	return true;
}


void fenParser(std::stringstream &fen, Position &board)
{
	std::vector<std::string> buffer{};
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

	for (ushort i = 0, r = RANK_8; i < buffer[0].length(); i++) { // loop until end of FEN string
		c = buffer[0][i]; // read a character from FEN string
		
		if (!(c == '/')) // is the end of the row in FEN string?
			switch (c) { // if not, compute the character
			case 'R':
				board.putPiece(WHITE, ROOK, Square((8 * r) + f));
				f++;
				break;
			case 'r':
				board.putPiece(BLACK, ROOK, Square((8 * r) + f));
				f++;
				break;
			case 'N':
				board.putPiece(WHITE, KNIGHT, Square((8 * r) + f));
				f++;
				break;
			case 'n':
				board.putPiece(BLACK, KNIGHT, Square((8 * r) + f));
				f++;
				break;
			case 'B':
				board.putPiece(WHITE, BISHOP, Square((8 * r) + f));
				f++;
				break;
			case 'b':
				board.putPiece(BLACK, BISHOP, Square((8 * r) + f));
				f++;
				break;
			case 'P':
				board.putPiece(WHITE, PAWN, Square((8 * r) + f));
				f++;
				break;
			case 'p':
				board.putPiece(BLACK, PAWN, Square((8 * r) + f));
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
	(buffer[1] == "w") ? board.setTurn(WHITE) : board.setTurn(BLACK); // who can move, black or white?
	
	//Castling rights. Extract from FEN, then write to the board
	ushort castle_white{}, castle_black{};
	
	for (ushort i = 0; i < buffer[2].length(); i++) {
		c = buffer[2][i];
		
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
	
	board.setCastle(WHITE, Castle(castle_white));
	board.setCastle(BLACK, Castle(castle_black));

	// En passant square
	if (buffer.size() > 3) {
		if (!(buffer[3] == "-")) {
			std::string epSq = buffer[3];
			std::transform(epSq.begin(), epSq.end(), epSq.begin(), ::tolower); // lower-case the en-passant value, just in case
			if (!(stringToSquareMap.find(epSq) == stringToSquareMap.end())) // is there a valid value, corresponding to map?
				board.setEnPassant(stringToSquareMap[epSq]);
			else
				board.setEnPassant(SQ_EMPTY);
		}
		else {
			board.setEnPassant(SQ_EMPTY);
		}
	}
	else {
		board.setEnPassant(SQ_EMPTY);
	}

	if (buffer.size() > 4)
		board.setHalfMove(std::stoi(buffer[4], nullptr, 0)); // set half move
	else
		board.setHalfMove(0);

	if (buffer.size() > 5)
		board.setMoveNumber(std::stoi(buffer[5], nullptr, 0)); // set move number
	else
		board.setMoveNumber(1);

	std::vector<Move> moveList = moveGeneration(board);
	pruneIllegal(moveList, board);
	// Reset checkmate: the position is valid, so it's not checkmate by default
	// It will be set to true later only if actually in checkmate or stalemate during search
	board.setCheckmate(false);
	
	board.setZobrist();
	
	// Only display when explicitly requested, not on every position set
	// displayBoard(board);
}
