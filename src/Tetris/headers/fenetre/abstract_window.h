#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_net.h>
#include <SDL_image.h>

//#define FULL_SCREEN
//#define SDL_MainHandled
#define FPS 300

struct Timing
{
	const double timeDifference;
	double time;
	double newTime;
	double dt;

	Timing();

	bool update();
};

/*
	Singleton.
*/
class AbstractWindow
{
private:
	static bool gameInstancied;
	const char* title;

protected:
	bool continuer;
	int width, height;
	Timing timing;
	SDL_Renderer* renderer;

public:
	AbstractWindow(const char* title);

	/*
		Lance le jeu.
		Cette fonction se termine uniquement
		apres la fermeture de la fenetre.
	*/
	void start();

protected:
	/*
		Fonction appelee lorsqu'un
		evenement est recupere.
	*/
	virtual void onSetup() = 0;

	/*
		Fonction appelee lorsqu'un
		evenement est recupere.
	*/
	virtual void onUpdateEvent(SDL_Event& event) = 0;

	/*
		Met a jour le jeu.
		Fonction appelee juste avant onDraw.
	*/
	virtual void onUpdate() = 0;

	/*
		Dessine le jeu a l'ecran.
		Fonction appelee FPS fois par secondes.
	*/
	virtual void onDraw() = 0;

	/*
		Fonction appelee apres la fermeture
		de la fenetre.
	*/
	virtual void onClose() = 0;

private:
	void loop();
};