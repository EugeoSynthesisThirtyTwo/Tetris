#pragma once

#include <functional>

#include "tetris/piece.h"
#include "tetris/gridcase.h"
#include "better_vector.h"

/*
  La grille du tetris.
*/
class Grid
{
private:
	GridCase* array;
	size_t width, height;

public:
	/* fonction appelee quand une ligne 'y' est effacee */
	std::function<void(int y)> onClearLine;
	
public:
	Grid();

	/* width, height : les dimensions de la grille */
	Grid(size_t width, size_t height);

	/* unserialize */
	Grid(const char* buffer, int& cursor);

	/* serialize les donnees de l'objet dans buffer */
	void serialize(BetterVector<char>& buffer) const;

	void operator=(Grid&& other) noexcept;

	~Grid();

	/* remplace toutes les cases par du vide */
	void clear();

	/* renvoie la case aux coordonnees (x, y) */
	GridCase& at(size_t x, size_t y);

	/* renvoie la case aux coordonnees (x, y) */
	const GridCase& at(size_t x, size_t y) const;

	/* renvoie la case a l'indice index */
	GridCase& operator[](size_t index);

	/* renvoie la case a l'indice index */
	const GridCase& operator[](size_t index) const;

	/* true si piece est en collision avec une ou plusieur cases */
	bool collision(const Piece& piece, int offset_x, int offset_y) const;

	/* incruste la piece dans la liste des cases */
	bool lockPiece(const Piece& piece, int offset_x, int offset_y);

	/* cherche les lignes a effacer, et les efface */
	void clearLines();

	/* rogne rect pour que chaque case soit un carre */
	void makeOrthonormal(SDL_Rect& rect);

	/* rogne rect pour que chaque case soit un carre
	pour une grille de dimensions (width, height) */
	static void makeOrthonormal(SDL_Rect& rect, size_t width, size_t height);

	/* rogne rect en conservant ses proportions
	pour qu'il rentre dans into */
	static void makeOrthonormal(SDL_Rect& rect, const SDL_Rect& into);

	/* renvoie le rectangle de la ligne line */
	SDL_Rect getLineRect(int line, const SDL_Rect& dest) const;

	/* dessine la grille a l'ecran */
	void render(SDL_Renderer* renderer, const SDL_Rect& dest) const;

	/* renvoie la longueur de la grille */
	size_t getWidth() const;

	/* renvoie la hauteur de la grille */
	size_t getHeight() const;
};
