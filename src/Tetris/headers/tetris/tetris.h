#pragma once

#include "tetris/basic_tetris.h"

class Game;

/* Un effet visuel quelconque. */
struct TetrisEffect
{
	/* true si l'effet est actif */
	bool active;
	/* instant du debut de l'effet en secondes */
	double startTime;
	/* duree de l'effet en secondes */
	double duration;

	TetrisEffect();

	/* unserialize */
	TetrisEffect(const char* buffer, int& cursor);

	/* serialize les donnees de l'objet dans buffer */
	void serialize(BetterVector<char>& buffer) const;

	/* desactive l'effet et remet les parametres par defaut */
	void reset();
};

struct Line
{
	TetrisEffect clearEffect;
	/* la coordonnee en y de la ligne */
	int line;
	/* les couleurs de la ligne */
	std::vector<Color> cases;

	Line();

	/* unserialize */
	Line(const char* buffer, int& cursor);

	/* serialize les donnees de l'objet dans buffer */
	void serialize(BetterVector<char>& buffer) const;
};

class Tetris : public BasicTetris
{
private:
	/* le logo de l'inversion gauche droite */
	TetrisEffect LRReversed;
	/* l'ensemble des animation des lignes a effacer*/
	std::vector<Line> lines;
	/* le jeu qui contient ce tetris */
	Game* game;

public:
	Tetris(int width, int height, Game* from);

	Tetris(const Tetris& other) = delete;

	Tetris(Tetris&& other) noexcept;

	/* unserialize */
	Tetris(const char* buffer, int& cursor, Game* from);

	void serialize(BetterVector<char>& buffer) const;

	void operator=(Tetris&& other) noexcept;

	bool down();

	bool right();

	bool left();

	bool rotate();

	bool update(double clockSecondes) = delete;

	bool update();

	/* active l'inversion gauche droite */
	void reverseLeftRight();

	/* active l'animation de l'effacement de la ligne y */
	void animationLineClear(int y);

	/* dessine le tetris a l'ecran */
	void render(SDL_Renderer* renderer, SDL_Rect dest) const;

	/* change le jeu qui contient ce tetris */
	void setGame(Game* from);

private:
	/* dessine la liste des bonus / malus */
	void renderEffects(SDL_Renderer* renderer, const SDL_Rect& dest) const;
};