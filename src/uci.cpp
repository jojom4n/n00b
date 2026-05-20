#include "uci.h"
#include "search.h"
#include "Position.h"
#include "movegen.h"
#include "makemove.h"
#include "display.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

namespace UCI {

    Position position;
    bool searchActive = false;

    void handleUciCommand() {
        std::cout << "id name n00b" << std::endl;
        std::cout << "id author Binary Pollution" << std::endl;
        std::cout << "option name Hash type spin default 256 min 16 max 512" << std::endl;
        std::cout << "uciok" << std::endl;
    }

    void handleIsReadyCommand() {
        std::cout << "readyok" << std::endl;
    }

    void handleSetOptionCommand(const std::string& line) {
        std::istringstream iss(line);
        std::string name, value;

        iss >> name;
        if (name == "name") {
            iss >> name;
            if (name == "Hash") {
                iss >> value >> value;
                // TODO: Implement hash size setting
            }
        }
    }

    void handleUciNewGameCommand() {
        position.resetPosition();
        position.setNew();
    }

    void handlePositionCommand(const std::string& line) {
        std::istringstream iss(line);
        std::string token;

        iss >> token;

        if (token == "startpos") {
            position.resetPosition();
            position.setNew();
        } else if (token == "fen") {
            // TODO: Implement FEN parsing and position setup
            position.resetPosition();
            position.setNew();
        }

        if (iss >> token && token == "moves") {
            std::string moveStr;
            while (iss >> moveStr) {
                // TODO: Parse algebraic notation and execute move
                // For now, just skip
            }
        }
    }

    void handleGoCommand(const std::string& line) {
        std::istringstream iss(line);
        std::string token;
        short depth = 6;

        while (iss >> token) {
            if (token == "depth") {
                iss >> depth;
            }
            // TODO: Handle wtime, btime, movestogo parameters
        }

        Search search;
        Move bestMove = iterativeSearch(position, search, depth);

        if (bestMove) {
            std::cout << "bestmove " << displayMove(position, bestMove) << std::endl;
        }
    }

    void handleStopCommand() {
        searchActive = false;
    }

    void handleQuitCommand() {
        exit(0);
    }

    void run() {
        std::string line;

        while (std::getline(std::cin, line)) {
            if (line.empty())
                continue;

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
        }
    }

} // namespace UCI
