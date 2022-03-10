#include <cstdlib>
#include <cmath>

#include "tetris/piece.h"
#include "network/serializer.h"

Piece::Piece()
{
	randomize();
}

Piece::Piece(const char* buffer, int& cursor)
{
	color = std::move(Color(buffer, cursor));

	for (int i = 0; i < 4; i++)
		coords_x[i] = unserializeInt(buffer, cursor);

	for (int i = 0; i < 4; i++)
		coords_y[i] = unserializeInt(buffer, cursor);

	type = unserializeInt(buffer, cursor);
	rotation = unserializeInt(buffer, cursor);
}

void Piece::serialize(BetterVector<char>& buffer) const
{
	color.serialize(buffer);

	for (int i = 0; i < 4; i++)
		serializeInt(buffer, coords_x[i]);

	for (int i = 0; i < 4; i++)
		serializeInt(buffer, coords_y[i]);

	serializeInt(buffer, type);
	serializeInt(buffer, rotation);
}

void Piece::operator=(const Piece& other)
{
	for (int i = 0; i < 4; i++)
	{
		coords_x[i] = other.coords_x[i];
		coords_y[i] = other.coords_y[i];
	}

	color = other.color;
	type = other.type;
	rotation = other.rotation;
}

void Piece::randomize()
{
	if (Piece::indexPioche == 7)
	{
		for (int i = 0; i < 7; i++)
			pioche[i] = i;

		static constexpr int NB_SHUFFLE = 50;
		int a, b, temp;

		for (int i = 0; i < NB_SHUFFLE; i++)
		{
			a = std::rand() % 7;
			b = std::rand() % 7;

			temp = pioche[a];
			pioche[a] = pioche[b];
			pioche[b] = temp;
		}

		Piece::indexPioche = 0;
	}

	type = pioche[Piece::indexPioche++];
	rotation = 0;
	construct();
}

void Piece::rotate(bool reverse)
{
	if (reverse)
		rotation = (rotation + 3) % 4;
	else
		rotation = (rotation + 1) % 4;

	construct();
}

void Piece::construct()
{
	switch (type)
	{
		// ligne
		case 0:
			// horizontale
			if (rotation % 2 == 0)
			{
				coords_x[0] = -1;
				coords_x[1] = 0;
				coords_x[2] = 1;
				coords_x[3] = 2;
				coords_y[0] = 1;
				coords_y[1] = 1;
				coords_y[2] = 1;
				coords_y[3] = 1;
			}
			// vertical
			else
			{
				coords_x[0] = 0;
				coords_x[1] = 0;
				coords_x[2] = 0;
				coords_x[3] = 0;
				coords_y[0] = -1;
				coords_y[1] = 0;
				coords_y[2] = 1;
				coords_y[3] = 2;
			}

			color = Color(255, 127, 0);
			break;

		// carre
		case 1:
			coords_x[0] = 0;
			coords_x[1] = 1;
			coords_x[2] = 0;
			coords_x[3] = 1;
			coords_y[0] = 0;
			coords_y[1] = 0;
			coords_y[2] = 1;
			coords_y[3] = 1;
			color = Color(255, 0, 0);
			break;

		// T
		case 2:
			// verticale
			if (rotation % 2 == 0)
			{
				coords_x[0] = -1;
				coords_x[1] = 0;
				coords_x[2] = 1;
				coords_y[0] = 0;
				coords_y[1] = 0;
				coords_y[2] = 0;
			}
			// horizontale
			else
			{
				coords_x[0] = 0;
				coords_x[1] = 0;
				coords_x[2] = 0;
				coords_y[0] = -1;
				coords_y[1] = 0;
				coords_y[2] = 1;
			}

			switch (rotation)
			{
				// bas
				case 0:
					coords_x[3] = 0;
					coords_y[3] = 1;
					break;
				// gauche
				case 1:
					coords_x[3] = -1;
					coords_y[3] = 0;
					break;
				// haut
				case 2:
					coords_x[3] = 0;
					coords_y[3] = -1;
					break;
				// droite
				case 3:
					coords_x[3] = 1;
					coords_y[3] = 0;
					break;
			}

			color = Color(255, 255, 0);
			break;

		// L
		case 3:
			switch (rotation)
			{
				// horizontale
				case 0:
					coords_x[0] = -1;
					coords_x[1] = -1;
					coords_x[2] = 0;
					coords_x[3] = 1;
					coords_y[0] = 1;
					coords_y[1] = 0;
					coords_y[2] = 0;
					coords_y[3] = 0;
					break;

				// verticale
				case 1:
					coords_x[0] = -1;
					coords_x[1] = 0;
					coords_x[2] = 0;
					coords_x[3] = 0;
					coords_y[0] = -1;
					coords_y[1] = -1;
					coords_y[2] = 0;
					coords_y[3] = 1;
					break;

				// horizontale
				case 2:
					coords_x[0] = -1;
					coords_x[1] = 0;
					coords_x[2] = 1;
					coords_x[3] = 1;
					coords_y[0] = 1;
					coords_y[1] = 1;
					coords_y[2] = 1;
					coords_y[3] = 0;
					break;

				// verticale
				case 3:
					coords_x[0] = 0;
					coords_x[1] = 0;
					coords_x[2] = 0;
					coords_x[3] = 1;
					coords_y[0] = -1;
					coords_y[1] = 0;
					coords_y[2] = 1;
					coords_y[3] = 1;
					break;
			}

			color = Color(0, 0, 255);
			break;

		// L mirroir
		case 4:
			switch (rotation)
			{
				// horizontale
				case 0:
					coords_x[0] = -1;
					coords_x[1] = 0;
					coords_x[2] = 1;
					coords_x[3] = 1;
					coords_y[0] = 0;
					coords_y[1] = 0;
					coords_y[2] = 0;
					coords_y[3] = 1;
					break;

				// verticale
				case 1:
					coords_x[0] = -1;
					coords_x[1] = 0;
					coords_x[2] = 0;
					coords_x[3] = 0;
					coords_y[0] = 1;
					coords_y[1] = 1;
					coords_y[2] = 0;
					coords_y[3] = -1;
					break;

				// horizontale
				case 2:
					coords_x[0] = -1;
					coords_x[1] = -1;
					coords_x[2] = 0;
					coords_x[3] = 1;
					coords_y[0] = 0;
					coords_y[1] = 1;
					coords_y[2] = 1;
					coords_y[3] = 1;
					break;

				// verticale
				case 3:
					coords_x[0] = 0;
					coords_x[1] = 0;
					coords_x[2] = 0;
					coords_x[3] = 1;
					coords_y[0] = 1;
					coords_y[1] = 0;
					coords_y[2] = -1;
					coords_y[3] = -1;
					break;
			}

			color = Color(255, 0, 255);
			break;

		// S
		case 5:
			// horizontale
			if (rotation % 2 == 0)
			{
				coords_x[0] = -1;
				coords_x[1] = 0;
				coords_x[2] = 0;
				coords_x[3] = 1;
				coords_y[0] = 1;
				coords_y[1] = 1;
				coords_y[2] = 0;
				coords_y[3] = 0;
			}
			// verticale
			else
			{
				coords_x[0] = -1;
				coords_x[1] = -1;
				coords_x[2] = 0;
				coords_x[3] = 0;
				coords_y[0] = -1;
				coords_y[1] = 0;
				coords_y[2] = 0;
				coords_y[3] = 1;
			}

			color = Color(0, 200, 255);
			break;

		// Z
		case 6:
			// horizontale
			if (rotation % 2 == 0)
			{
				coords_x[0] = -1;
				coords_x[1] = 0;
				coords_x[2] = 0;
				coords_x[3] = 1;
				coords_y[0] = 0;
				coords_y[1] = 0;
				coords_y[2] = 1;
				coords_y[3] = 1;
			}
			// verticale
			else
			{
				coords_x[0] = 0;
				coords_x[1] = 0;
				coords_x[2] = 1;
				coords_x[3] = 1;
				coords_y[0] = 1;
				coords_y[1] = 0;
				coords_y[2] = 0;
				coords_y[3] = -1;
			}

			color = Color(0, 255, 0);
			break;
	}
}

int Piece::pioche[7];
int Piece::indexPioche = 7;