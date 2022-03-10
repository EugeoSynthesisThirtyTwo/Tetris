#pragma once

#include <SDL.h>

#include "fenetre/color.h"
#include "better_vector.h"

/*
  Contient les informations d'une case de la grille.
*/
class GridCase
{
public:
	/* la couleur par defaut d'une case */
	static Color DEFAULT_COLOR;
	/* true si c'est une case pleine
	false si c'est de l'air */
	bool filled;
	/* la couleur de la case */
	Color color;

	GridCase();

	GridCase(bool filled, Color color);

	/* unserialize */
	GridCase(const char* buffer, int& cursor);

	/* serialize les donnees de l'objet dans buffer */
	void serialize(BetterVector<char>& buffer) const;

	/* renvoie color si la case est pleine
	renvoie DEFAULT_COLOR sinon */
	Color getTrueColor() const;
};

