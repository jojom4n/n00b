#include "pch.h"
#include "defs.h"
#include "protos.h"


Square operator++(Square &s, int) {
	Square temp = s;
	s = static_cast<Square>((static_cast<int>(s) + 1));
	return temp;
}


Square operator--(Square &s, int) {
	Square temp = s;
	s = static_cast<Square>((static_cast<int>(s) - 1));
	return temp;
}


File operator++(File &f, int) {
	File temp = f;
	f = static_cast<File>((static_cast<int>(f) + 1));
	return temp;
}


File operator--(File &f, int) {
	File temp = f;
	f = static_cast<File>((static_cast<int>(f) - 1));
	return temp;
}


Rank operator++(Rank &r, int) {
	Rank temp = r;
	r = static_cast<Rank>((static_cast<int>(r) + 1));
	return temp;
}


Rank operator--(Rank &r, int) {
	Rank temp = r;
	r = static_cast<Rank>((static_cast<int>(r) - 1));
	return temp;
}


Piece operator++(Piece &p, int) {
	Piece temp = p;
	p = static_cast<Piece>((static_cast<int>(p) + 1));
	return temp;
}
