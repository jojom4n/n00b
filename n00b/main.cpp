#include "pch.h"
#include <iostream>
#include "protos.h"

int main()
{
	initAttacks();
	Zobrist::init();
	std::cout << "Welcome to n00b v. 0.01 - An experimental chess engine project" << std::endl;
	std::cout << "Copyright (c) 2019 Binary Pollution" << std::endl;
	newGame();
	return 0;
}