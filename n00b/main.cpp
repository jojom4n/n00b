#include "pch.h"
#include <iostream>
#include <thread>
#include "protos.h"
#include "defs.h"

int main()
{
	initAttacks();
	Zobrist::init();
	/* std::thread t3(TT::init);
	t3.detach(); */
	std::cout << "Welcome to n00b v. 0.01 - An experimental chess engine project" << std::endl;
	std::cout << "Copyright (c) 2019 Binary Pollution" << std::endl;
	newGame();
	return 0;
}