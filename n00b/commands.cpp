#include "pch.h"
#include <iostream>
#include <string>
#include <algorithm> // for std::transform in read_commands()
#include <limits> // for using limits in cin.ignore() - see code below
#undef max // for using "max()" identifier in cin.ignore - see code below
#include "protos.h"

void read_commands() 
{	
	do 
	{
		std::cout << "> ";
		std::cin >> input;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // take only first word

		 // transform the input in lowercase prior to examine it
		std::transform(input.begin(), input.end(), input.begin(), ::tolower);

		switch (resolve_input(input)) {
		case quit:
			break;
		case help:
			std::cout << "Sorry, no help included yet.\n" << std::endl;
			break;
		case fen:
			std::cout << "Sorry, function not implemented yet.\n" << std::endl;
			break;
		case play:
			new_game();
			break;
		default:
			std::cout << "Sorry, unavailable command.\n" << std::endl;
			break;
		}

	} while (!(input == "quit") && !(input == "q"));
}

Option resolve_input(std::string const &input)
{
	if (input == "quit" || input == "q")
		return quit;
	else if (input == "help" || input == "h" || input == "?")
		return help;
	else if (input == "fen" || input == "f")
		return fen;
	else if (input == "play" || input == "n")
		return play;
	else
		return invalid;
}