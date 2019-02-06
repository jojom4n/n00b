#ifndef PLAYER_H
#define PLAYER_H

#include "defs.h"

class Player
{
	bool checkmated = false; // is the player checkmated?
	unsigned short time = 0; // time available for the player
	Castle castling = castle_both; // can the player castle?
	bool move = true; // is the turn of the player?

public:
	Player();
	~Player();
	bool is_checkmated() const { return checkmated; } // get function
	void is_checkmated(bool const &b) { checkmated = b; } // set function
	Castle has_castling() const { return castling; } // get function
	void has_castling(Castle const &i) { castling = i; } // set function
	bool has_move() const { return move; } // get function
	void has_move(bool const &b) { move = b; } // set function
	unsigned short Time() const { return time; } // get function
	void Time(unsigned short const &i) { time = i; } // set function
};

#endif
