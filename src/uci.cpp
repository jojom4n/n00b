#include "uci.h"
#include "search.h"
#include "Position.h"
#include "movegen.h"
#include "makemove.h"
#include "display.h"
#include "globals.h"
#include "book.h"
#include "fen.h"
#include "tt.h"
#include "bitscan.h"
#include "overloading.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cassert>

namespace UCI {

    Position position;
    bool searchActive = false;

    // Forward declarations
    Move parseUCIMove(const std::string& moveStr);

    void handleUciCommand() {
        std::cout << "id name n00b" << std::endl;
        std::cout << "id author Binary Pollution" << std::endl;
        std::cout << "option name Hash type spin default 256 min 16 max 8192" << std::endl;
        std::cout << "option name Threads type spin default 1 min 1 max 512" << std::endl;
        std::cout << "option name Book type check default true" << std::endl;
        std::cout << "option name BookPath type string default books/book.bin" << std::endl;
        std::cout << "option name UseDefaultBook type check default true" << std::endl;
        std::cout << "uciok" << std::endl;
    }

    void handleIsReadyCommand() {
        std::cout << "readyok" << std::endl;
    }

    void handleSetOptionCommand(const std::string& line) {
        std::istringstream iss(line);
        std::string name, value, part;

        // Parse "name <name> [value <value>]"
        iss >> name; // skip "name"

        if (name != "name") {
            return; // malformed command
        }

        iss >> name; // get option name

        if (name == "Hash") {
            iss >> value; // skip "value"
            iss >> value; // get hash value
            int mb = std::stoi(value);
            TT::Init(); // Initialize transposition table with the hash size
        }
        else if (name == "Threads") {
            iss >> value; // skip "value"
            iss >> value; // get threads value
            // Note: search_threads is not defined in globals.h
            // This would need to be added to params.h or globals.h
        }
        else if (name == "Book") {
            iss >> value; // skip "value"
            iss >> value; // get book enable/disable value
            bool useBook = (value == "true");
            set_use_book(useBook);
            if (!useBook) {
                g_book_manager.clearBook();
            }
        }
        else if (name == "BookPath") {
            iss >> value; // skip "value"
            iss >> value; // get book path
            set_book_path(value);
        }
        else if (name == "UseDefaultBook") {
            iss >> value; // skip "value"
            iss >> value; // get use default book value
            bool useDefault = (value == "true");
            set_use_default_book(useDefault);
        }
    }

    void handleUciNewGameCommand() {
        position.resetPosition();
        position.setNew();
    }

    // Helper to convert UCI square notation to Square enum value
    static Square uciStringToSquare(const std::string& sq) {
        if (sq.length() < 2) return SQ_EMPTY;
        int file = sq[0] - 'a';
        int rank = sq[1] - '1';
        if (file < 0 || file > 7 || rank < 0 || rank > 7) return SQ_EMPTY;
        return Square(rank * 8 + file);
    }

    // Helper to find which piece is at 'from' square for the side to move
    static Piece findPieceAtSquare(Square from, Color turn, const Position& pos) {
        // Iterate from KING(0) to PAWN(5) to cover all piece types
        for (Piece piece = KING; piece <= PAWN; piece++) {
            Bitboard bb = pos.getPieces(turn, piece);
            while (bb) {
                Square sq = Square(bitscan_reset(bb));
                if (sq == from) return piece;
            }
        }
        return NO_PIECE;
    }

    void handlePositionCommand(const std::string& line) {
        std::istringstream iss(line);
        std::string token;

        // First, check what type of position we have
        iss >> token;

        if (token == "startpos") {
            // Reset to the starting position
            position.resetPosition();
            position.setNew();
            
            // Now check for moves
            std::string movesToken;
            if (iss >> movesToken && movesToken == "moves") {
                std::string moveStr;
                while (iss >> moveStr) {
                    Move move = parseUCIMove(moveStr);
                    if (move) {
                        ::doMove(move, position);
                    }
                }
            }
        }
        else if (token == "fen") {
            // Read the full FEN string until we hit "moves" or end of line
            std::string fenStr;
            std::string word;
            bool gotPosition = false;
            while (iss >> word) {
                if (word == "moves") {
                    // Found moves, parse them after
                    std::string moveStr;
                    while (iss >> moveStr) {
                        Move move = parseUCIMove(moveStr);
                        if (move) {
                            ::doMove(move, position);
                        }
                    }
                    break;
                }
                if (gotPosition) {
                    fenStr += " ";
                }
                fenStr += word;
                gotPosition = true;
            }

            if (gotPosition) {
                // Parse the FEN string
                std::stringstream fenSS(fenStr);
                ::fenParser(fenSS, position);
            }
        }
    }

    Move parseUCIMove(const std::string& moveStr) {
        // Parse UCI format move (e.g., "e2e4", "g1f3", "e1g1" for castling)
        if (moveStr.length() < 4) {
            return Move(0);
        }

        // Convert UCI square notation to Square enum values
        std::string fromSq = moveStr.substr(0, 2);
        std::string toSq = moveStr.substr(2, 2);
        Square from = uciStringToSquare(fromSq);
        Square to = uciStringToSquare(toSq);

        if (from == SQ_EMPTY || to == SQ_EMPTY) {
            return Move(0);
        }

        // Determine the side to move
        Color turn = position.getTurn();

        // Find the piece at the from square
        Piece piece = findPieceAtSquare(from, turn, position);
        if (piece == NO_PIECE) {
            return Move(0);
        }

        // Check if the to square has an enemy piece (capture)
        Piece captured = NO_PIECE;
        for (Color c = BLACK; c <= WHITE; c++) {
            if (c != turn) {
                // Check all piece types for the enemy
                for (Piece piece = KING; piece <= PAWN; piece++) {
                    Bitboard pieceBB = position.getPieces(c, piece);
                    if (pieceBB & (C64(1) << to)) {
                        captured = piece;
                        break;
                    }
                }
                if (captured != NO_PIECE) break;
            }
        }

        // Determine move type
        MoveType moveType = QUIET;
        
        // Check for castling by comparing from/to squares
        if (piece == KING) {
            int fileDiff = std::abs(int(to % 8) - int(from % 8));
            if (fileDiff == 2) {
                // Possible castling
                moveType = (to > from) ? CASTLE_K : CASTLE_Q;
                return ::composeMove(from, to, turn, piece, moveType, captured, 0);
            }
        }

        // Check for pawn promotion
        if (moveStr.length() >= 5 && piece == PAWN) {
            char promoChar = moveStr[4];
            switch (promoChar) {
                case 'n': case 'N': moveType = PROMOTION; break;
                case 'q': case 'Q': moveType = PROMOTION; break;
                case 'r': case 'R': moveType = PROMOTION; break;
                case 'b': case 'B': moveType = PROMOTION; break;
                default: break;
            }
            if (moveType == PROMOTION) {
                uint8_t promoType = 0;
                switch (promoChar) {
                    case 'n': case 'N': promoType = PAWN_TO_KNIGHT; break;
                    case 'b': case 'B': promoType = PAWN_TO_BISHOP; break;
                    case 'r': case 'R': promoType = PAWN_TO_ROOK; break;
                    case 'q': case 'Q': promoType = PAWN_TO_QUEEN; break;
                }
                return ::composeMove(from, to, turn, piece, moveType, captured, promoType);
            }
        }

        // Check for en passant
        if (piece == PAWN && captured == PAWN) {
            Square ep = position.getEnPassant();
            if (to == ep) {
                moveType = EN_PASSANT;
                return ::composeMove(from, to, turn, piece, moveType, captured, 0);
            }
        }

        // Determine if it's a capture or quiet move
        if (captured != NO_PIECE) {
            moveType = CAPTURE;
        }

        // For pawns moving two squares
        // (the move type is already correct, just need to handle en passant square setting in doMove)

        return ::composeMove(from, to, turn, piece, moveType, captured, 0);
    }

    void handleGoCommand(const std::string& line) {
        std::istringstream iss(line);
        std::string token;
        short depth = 6;

        while (iss >> token) {
            if (token == "depth") {
                iss >> depth;
            }
            else if (token == "movestogo") {
                // TODO: Handle movestogo parameter
            }
            else if (token == "wtime") {
                // TODO: Handle white time
            }
            else if (token == "btime") {
                // TODO: Handle black time
            }
            else if (token == "winc") {
                // TODO: Handle white increment
            }
            else if (token == "binc") {
                // TODO: Handle black increment
            }
        }

        // Check if book is enabled and loaded
        if (get_use_book() && g_book_manager.isBookLoaded()) {
            // Get book move if available
            auto bookMoveOpt = g_book_manager.getWeightedBookMove(position);
            if (bookMoveOpt.has_value()) {
                Move bookMove = bookMoveOpt.value();
                std::cout << "bestmove " << displayMove(position, bookMove) << std::endl;
                return;
            }
        }

        Search search;
        Move bestMove = iterativeSearch(position, search, depth);

        if (bestMove) {
            std::cout << "bestmove " << displayMove(position, bestMove) << std::endl;
        }
        else {
            std::cout << "bestmove 0000" << std::endl;
        }
    }

    void handleStopCommand() {
        searchActive = false;
    }

    void handleQuitCommand() {
        exit(0);
    }

    void run() {
        // Initialize book manager if default book is enabled
        init_book_manager();

        std::string line;

        while (std::getline(std::cin, line)) {
            if (line.empty())
                continue;

            // Trim trailing whitespace
            size_t end = line.find_last_not_of(" \t\r\n");
            if (end != std::string::npos) {
                line.erase(end + 1);
            }

            std::istringstream iss(line);
            std::string command;
            iss >> command;

            if (command == "uci") {
                handleUciCommand();
            }
            else if (command == "isready") {
                handleIsReadyCommand();
            }
            else if (command == "setoption") {
                std::string rest;
                std::getline(iss, rest);
                handleSetOptionCommand(rest);
            }
            else if (command == "ucinewgame") {
                handleUciNewGameCommand();
            }
            else if (command == "position") {
                std::string rest;
                std::getline(iss, rest);
                handlePositionCommand(rest);
            }
            else if (command == "go") {
                std::string rest;
                std::getline(iss, rest);
                handleGoCommand(rest);
            }
            else if (command == "stop") {
                handleStopCommand();
            }
            else if (command == "quit") {
                handleQuitCommand();
            }
            else if (command == "ping") {
                // Debug command - just respond readyok
                std::cout << "readyok" << std::endl;
            }
        }
    }

} // namespace UCI