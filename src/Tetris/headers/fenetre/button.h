#pragma once

#include <string>
#include <functional>

#include <SDL.h>
#include <SDL_ttf.h>

#include "fenetre/color.h"

/*
  Bouton rectangulaire monochrome.
*/
class Button
{
protected:
	static const Color DEFAULT_COLOR;
	int mouse_x, mouse_y;
	bool mouseSet;
	bool pressed;

public:
	SDL_Rect dest;
	std::string text;
	Color colorBackground;
	Color colorText;
	std::function<void()> action;

public:
	/*
	  dest : position du bouton sur la fenetre
	*/
	Button(const SDL_Rect& dest);

	/*
	  dest : position du bouton sur la fenetre
	  text : texte du bouton
	*/
	Button(const SDL_Rect& dest, const std::string& text);

	/*
	  event : l'evenement qui met a jour le bouton
	*/
	virtual void update(const SDL_Event& event);

	/*
	  dessine le bouton a l'ecran
	  font : la police de caracteres utilisee
	*/
	virtual void render(SDL_Renderer* renderer, TTF_Font* font) const;

	/*
	  Fonction a appeller apres avoir utilise la fonction SDL_RenderCopy.
	  Sinon plus rien ne peut etre dessine sur le renderer.
	*/
	static void debugTexture(SDL_Renderer* renderer);

protected:
	/*
	  renvoie true si la souris est a l'interieur du bouton.
	*/
	bool inside() const;
};