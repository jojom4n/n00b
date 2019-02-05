#include "pch.h"
#include <iostream>
#include <string>
#include "board.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

int main()
{
	string fen = "";
	cout << "Welcome to N00b v. 0.1 - An experimental chess project" << endl;
	cout << "Copyright (c) 2019 Binary Pollution" << endl;
	cout << "Please enter FEN: ";
	cin >> fen;
	/* Test */ cout << "FEN is " << fen << endl;
	return 0;
}