#include "pch.h"
#include <iostream>
#include <string>
#include <algorithm> // for std::transform in read_commands()
#include <Windows.h>
#include "protos.h"

void read_commands() 
{
	std::string buffer;
	
	do 
	{
		std::cout << ">>>> ";
		std::cin >> buffer;

		std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::tolower); // transforms the buffer in lowercase prior to examine it

		switch (resolve_buffer(buffer)) {
		case quit:
			break;
		case help:
			std::cout << "Sorry, no help included yet.\n" << std::endl;
			break;
		case fen:
			std::cout << "Sorry, function not implemented yet.\n" << std::endl;
			break;
		case play:
			std::cout << "Sorry, function not implemented yet.\n" << std::endl;
			break;
		case sysinfo:
			std::cout << "Sorry, function not implemented yet.\n" << std::endl;
			break;
		case display:
			std::cout << "Sorry, function not implemented yet.\n" << std::endl;
			break;
		default:
			std::cout << "Sorry, unavailable command.\n" << std::endl;
			break;
		}
	} while (!(buffer == "quit") && !(buffer == "q"));
}

Options resolve_buffer(const std::string const& buffer) {
	if (buffer == "quit" | buffer == "q")
		return quit;
	else if (buffer == "help" | buffer == "h" | buffer == "?")
		return help;
	else if (buffer == "fen" | buffer == "f")
		return fen;
	else if (buffer == "play" | buffer == "p")
		return play;
	else if (buffer == "sysinfo" | buffer == "s")
		return sysinfo;
	else if (buffer == "display" | buffer == "d")
		return display;
	else
		return invalid;
}