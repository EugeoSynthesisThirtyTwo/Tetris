#pragma once

#include <vector>

#include <SDL.h>

#include "better_vector.h"

/*
  Fait la meme chose que SDL_Color,
  mais peut etre serialize.
*/
class Color
{
public:
	Uint8 r, g, b, a;

	Color();

	Color(Uint8 r, Uint8 g, Uint8 b);

	Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* unserialize */
	Color(const char* buffer, int& cursor);

	/* serialize les donnees de l'objet dans buffer */
	void serialize(BetterVector<char>& buffer) const;
};

