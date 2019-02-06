#ifndef PLAYER_H
#define PLAYER_H

#include "defs.h" // for constexpr "white" and "black"

class Player
{
	bool checkmated;
	unsigned short time;
	bool castling;
	bool move;

public:
	Player();
	~Player();
	bool is_Checkmated() const { return checkmated; } // get function
	void is_Checkmated(bool const& b) { checkmated = b; } // set function
	bool has_Castling() const { return castling; } // get function
	void has_Castling(bool const& b) { castling = b; } // set function
	bool has_Move() const { return move; } // get function
	void has_Move(bool const& b) { move = b; } // set function
};

#endif
