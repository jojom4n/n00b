#include "pch.h"
#include <iostream>
#include <thread>
#include "protos.h"

int main()
{
	std::thread t1 (initAttacks);
	t1.detach();
	std::thread t2 (Zobrist::init);
	t2.detach();
	/* std::thread t3(TT::init);
	t3.detach(); */
	std::cout << "Welcome to n00b v. 0.01 - An experimental chess engine project" << std::endl;
	std::cout << "Copyright (c) 2019 Binary Pollution" << std::endl;
	newGame();
	return 0;
}