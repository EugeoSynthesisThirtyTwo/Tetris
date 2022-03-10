#include <SDL.h>
#include <SDL_image.h>

#include "fenetre/ressources.h"

int Ressources::init()
{
	if (initialized)
		return 0;

	font = TTF_OpenFont("ressources/fonts/arial.ttf", 300);

	if (font == nullptr)
	{
		error = TTF_GetError();
		return -1;
	}

	inversion = IMG_Load("ressources/images/inversion.png");

	if (inversion == nullptr)
	{
		error = IMG_GetError();
		return -1;
	}

	if (SDL_LoadWAV("ressources/musique/tetris.wav", &wav_spec, &wav_buffer, &wav_length) == nullptr)
	{
		error = SDL_GetError();
		return -1;
	}

	initialized = true;
	return 0;
}

void Ressources::quit()
{
	SDL_FreeWAV(wav_buffer);
	SDL_FreeSurface(inversion);
	inversion = nullptr;
	TTF_CloseFont(font);
	font = nullptr;

	initialized = false;
}

const char* Ressources::GetError()
{
	return error;
}

bool Ressources::initialized = false;
const char* Ressources::error = "No error";

TTF_Font* Ressources::font = nullptr;
SDL_Surface* Ressources::inversion = nullptr;

SDL_AudioSpec Ressources::wav_spec;
Uint32 Ressources::wav_length = 0;
Uint8* Ressources::wav_buffer = nullptr;