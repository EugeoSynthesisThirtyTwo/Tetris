#include <iostream>

#include "fenetre/abstract_window.h"
#include "fenetre/ressources.h"

// Timing

Timing::Timing()
	: timeDifference(1.0 / FPS)
{
	time = SDL_GetTicks() / 1000.0 - timeDifference;
	newTime = 0;
	dt = 0;
}

bool Timing::update()
{
	newTime = SDL_GetTicks() / 1000.0;
	dt = newTime - time;

	if (dt >= timeDifference)
	{
		time = newTime;
		return true;
	}

	return false;
}

// AbstractGame

AbstractWindow::AbstractWindow(const char* title)
{
	if (AbstractWindow::gameInstancied)
	{
		std::cerr << "Game ne dois etre instancie qu'une seule fois." << SDL_GetError();
		exit(EXIT_FAILURE);
	}

	this->title = title;
	continuer = true;
	width = 0;
	height = 0;
	renderer = nullptr;
	AbstractWindow::gameInstancied = true;
}

void AbstractWindow::start()
{
	#ifdef SDL_MainHandled
	SDL_SetMainReady();
	#endif

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		std::cerr << "Echec de l'initalisation de la SDL : " << SDL_GetError();
		exit(EXIT_FAILURE);
	}

	if (TTF_Init() != 0)
	{
		std::cerr << "Echec de l'initalisation de la SDL_ttf : " << TTF_GetError();
		exit(EXIT_FAILURE);
	}

	if (SDLNet_Init() != 0)
	{
		std::cerr << "Echec de l'initalisation de SDL_net : " << SDLNet_GetError();
		exit(EXIT_FAILURE);
	}

	int IMG_flags = IMG_INIT_JPG | IMG_INIT_PNG;

	if (IMG_Init(IMG_flags) != IMG_flags)
	{
		std::cerr << "Echec de l'initalisation de SDL_image : " << IMG_GetError();
		exit(EXIT_FAILURE);
	}

	if (Ressources::init() != 0)
	{
		std::cerr << "Erreur du chargement des ressources : " << Ressources::GetError();
		exit(EXIT_FAILURE);
	}

	#ifdef FULL_SCREEN

	SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);

	#else

	SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);

	#endif

	if (window == nullptr)
	{
		std::cerr << "Echec de la creation de la fenetre : " << SDL_GetError();
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == nullptr)
	{
		std::cerr << "Erreur a la creation du renderer : " << SDL_GetError();
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_GetRendererOutputSize(renderer, &width, &height);

	onSetup();
	loop();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	Ressources::quit();
	IMG_Quit();
	SDLNet_Quit();
	TTF_Quit();
	SDL_Quit();
}

void AbstractWindow::loop()
{
	SDL_Event event;

	while (continuer)
	{
		timing.update();

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				continuer = false;

			onUpdateEvent(event);
		}

		if (timing.update())
		{
			onUpdate();
			onDraw();
			SDL_RenderPresent(renderer);
		}
	}

	onClose();
}

bool AbstractWindow::gameInstancied = false;