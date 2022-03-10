#pragma once

#include <string>

#include "tetris/grid.h"
#include "tetris/piece.h"

/*
  Un tetris basique, sans bonus / malus, sans
  effet visuel particulier, mais avec un pseudo.
*/
class BasicTetris
{
protected:
	/* Les dimensions de la grille. */
	size_t width, height;
	/* La grille. */
	Grid grid;
	/* La piece actuelle que le joueur manipule. */
	Piece currentPiece;
	/* La prochaine piece. */
	Piece nextPiece;
	/* Le decalage en x et y de la piece sur la grille. */
	int offset_x, offset_y;
	/* true si la partie n'est pas finie. */
	bool alive;
	/* L'instant de la derniere update en secondes. */
	double lastUpdate;
	/* Le delaie en secondes entre deux updates. */
	double delayUpdate;
	/* true quand il n'y a eu aucune update. */
	bool firstUpdate;

public:
	/* Cases fantomes a droite de la grille pour dessiner
	  d'autres choses. */
	static constexpr int paddingRight = 5;
	/* Pseudo du joueur. */
	std::string pseudo;

public:
	/* width et height : dimensions de la grille */
	BasicTetris(int width, int height);

	BasicTetris(const BasicTetris& other) = delete;

	BasicTetris(BasicTetris&& other) noexcept;

	/* unserialize */
	BasicTetris(const char* buffer, int& cursor);

	/* serialize les donnees de l'objet dans buffer */
	void serialize(BetterVector<char>& buffer) const;

	void operator=(BasicTetris&& other) noexcept;

	/* decale la piece d'une case vers le bas */
	bool down();

	/* decale la piece d'une case vers la droite */
	bool right();

	/* decale la piece d'une case vers la gauche */
	bool left();

	/* tourne la piece d'un quart de tour dans
	le sens horaire */
	bool rotate();

	/* true si la partie n'est pas finie. */
	bool isAlive() const;

	/*
	  Met a jour le jeu tous 'delayUpdate' secondes.
	  clockSecondes : horloge actuelle en secondes.
	  Renvoie true si il y a bien eu une update.
	*/
	bool update(double clockSecondes);

	/*
	  Definie l'action a realiser quand une ligne est
	  effacee.
	  y : la coordonnee y de la ligne dans grid.
	*/
	void setOnClearLine(std::function<void(int y)> onClearLine);

protected:
	/* Dessine une piece */
	void renderPiece(SDL_Renderer* renderer, const SDL_Rect& dest, const Piece& piece, int offset_x, int offset_y) const;

public:
	/* Dessine le tetris a l'ecran. */
	SDL_Rect render(SDL_Renderer* renderer, SDL_Rect dest) const;

	/* Renvoie le rectangle dans lequel est dessine grid. */
	SDL_Rect getGridClipRect(SDL_Rect dest) const;

private:
	/* Ecrit le pseudo du joueur a cote de la grille */
	void renderPseudo(SDL_Renderer* renderer, const SDL_Rect& dest) const;
};
