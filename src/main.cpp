#include "pch.h"
#include "attack.h"
#include "nnue_eval.h"
#include "game.h"
#include "tt.h"
#include "zobrist.h"
#include <iostream>
#include "uci.h"

int main()
{
	initAttacks();
	Zobrist::init();
	TT::Init();
	std::cout << "Welcome to n00b v. 0.1 - An experimental chess engine project\n";
	std::cout << "Copyright (c) 2021 Binary Pollution\n\n";
	init_NNUE("nn-62ef826d1a6d.nnue");
	UCI::run();
	return 0;
}
