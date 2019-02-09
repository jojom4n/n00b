#include "pch.h"
#include <iostream>
#include "protos.h"

std::string input = " ";

int main()
{
	init_masks();
	std::cout << "Welcome to n00b v. 0.1 - An experimental chess engine project" << std::endl;
	std::cout << "Copyright (c) 2019 Binary Pollution" << std::endl;
	new_game();
	return 0;
}