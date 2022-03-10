#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

/*
  singleton
*/
struct Ressources
{
private:
	static bool initialized;
	static const char* error;

public:
	static TTF_Font* font;
	static SDL_Surface* inversion;

	static SDL_AudioSpec wav_spec;
	static Uint32 wav_length;
	static Uint8* wav_buffer;

public:
	/*
	  charge le contenu du dossier ressources
	  renvoie 0 si il n'y a eu aucune erreur
	*/
	static int init();

	/*
	  decharge les ressources
	*/
	static void quit();

	/*
	  renvoie les details de la derniere erreur
	*/
	static const char* GetError();
};