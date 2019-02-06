#ifndef PLAYER_H
#define PLAYER_H

class Player
{
	bool checkmated; // is the player checkmated?
	unsigned short time; // time available for the player
	bool castling; // can the player castle?
	bool move; // is the turn of the player?

public:
	Player();
	~Player();
	bool is_Checkmated() const { return checkmated; } // get function
	void is_Checkmated(bool const& b) { checkmated = b; } // set function
	bool has_Castling() const { return castling; } // get function
	void has_Castling(bool const& b) { castling = b; } // set function
	bool has_Move() const { return move; } // get function
	void has_Move(bool const& b) { move = b; } // set function
	unsigned short Time() const { return time; } // get function
	void Time(unsigned short const& i) { time = i; } // set function
};

#endif
