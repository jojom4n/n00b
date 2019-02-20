#include "pch.h"
#include "Evaluation.h"


int Evaluation::material(Position const &pos)
{
	return 0;
}

int Evaluation::pieceSquareTable(Position const &pos)
{
	return 0;
}

Evaluation::Evaluation()
{
	material_.fill({});
}


Evaluation::~Evaluation()
{
}

int Evaluation::getScore(Position const &pos)
{
	return 0;
}
