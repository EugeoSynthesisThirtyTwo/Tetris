#pragma once

#include <string>

#include "fenetre/button.h"

/*
  Champ de texte.
*/
class Field: private Button
{
protected:
	bool focus;
	bool done;
	
public:
	std::function<void(std::string)> action;

public:
	Field(const SDL_Rect& rect);
	
	void update(const SDL_Event& event);

	/*
	  dessine le champ a l'ecran
	  font : la police de caracteres utilisee
	*/
	void render(SDL_Renderer* renderer, TTF_Font* font) const;

	/*
	  renvoie le texte entre par l'utilisateur
	*/
	std::string getText() const;

	/*
	  si focus = true alors du texte peut etre entre
	  sinon le clavier n'agira pas sur l'objet.
	*/
	void setFocus(bool focus);

	/*
	  convertie un symbole en caractere
	*/
	static char getKeyChar(int sym);
};
