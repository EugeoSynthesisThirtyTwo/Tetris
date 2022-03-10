#pragma once

#include "color.h"

class Piece
{
public:
	Color color;
	int coords_x[4];
	int coords_y[4];

private:
	int type;
	int rotation;
	static int pioche[7];
	static int indexPioche;

public:
	Piece();

	Piece(const Piece& other) = delete;

	/* unserialize */
	Piece(const char* buffer, int& cursor);

	void serialize(BetterVector<char>& buffer) const;

	void operator=(const Piece& other);

	void randomize();

	void rotate(bool reverse = false);

	void construct();
};

