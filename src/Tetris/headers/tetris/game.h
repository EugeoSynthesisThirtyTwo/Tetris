#pragma once

#include <mutex>
#include <functional>

#include <SDL.h>

#include "tetris/tetris.h"

/*
  Contient l'ensemble des donnees de
  la partie en cours.
*/
class Game
{
private:
	/* la liste des joueurs */
	std::vector<Tetris> tetris;
	mutable std::mutex mutex;
	mutable double clockSecondes;
	bool started;

public:
	static const int DEFAULT_WIDTH = 12, DEFAULT_HEIGHT = 20;

public:
	Game();

	/* unserialize */
	Game(const char* buffer, int& cursor);

	/* serialize les donnees de l'objet dans buffer */
	void serialize(BetterVector<char>& buffer) const;

	void operator=(Game&& other) noexcept;

	/* ajoute un joueur */
	void addTetris();

	/*
	  Met a jour l'ensemble des joueurs.
	  Cette fonction n'est execute que si
	  start a ete appellee.
	*/
	void update(double clockSecondes);

	/*
	  Appelle la fonction action avec en parametre
	  la liste des joueurs.
	*/
	void performAction(std::function<void(std::vector<Tetris>& tetris)> action);

	/*  Dessine le jeu a l'ecran */
	void render(SDL_Renderer* renderer, SDL_Rect dest) const;

	/* renvoie l'horloge en secondes du jeu */
	double getClockSecondes() const;

	/* lance la partie */
	void start();

	/* true si la fonction start a ete appellee */
	bool isStarted() const;

private:
	void setupOnClear(int index);
};