#include "pch.h"
#include "attack.h"
#include "game.h"
#include "zobrist.h"
#include <iostream>


int main()
{
	initAttacks();
	Zobrist::init();
	std::cout << "Welcome to n00b v. 0.1 - An experimental chess engine project" << std::endl;
	std::cout << "Copyright (c) 2021 Binary Pollution" << std::endl;
	newGame();
	return 0;
}