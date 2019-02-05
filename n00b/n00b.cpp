#include <iostream>
#include <string>
#include "pch.h"
#include "board.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

int main()
{
	string fen = "";
	cout << "Welcome to N00b v. 0.1 - An experimental chess engine project" << endl;
	cout << "Copyright (c) 2019 Binary Pollution" << endl;
	cout << "Please enter FEN: ";
	cin >> fen;
	
	// TODO: check for FEN validity

	Board* chessboard = new Board();
	chessboard->init(fen);
	return 0;
}