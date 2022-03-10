#include <iostream>
#include <ctime>

#include "fenetre/abstract_window.h"
#include "tetris/tetris.h"
#include "fenetre/field.h"
#include "network/server.h"
#include "network/client.h"
#include "fenetre/ressources.h"

class Window: public AbstractWindow
{
private:
	bool musicStarted;
	SDL_AudioDeviceID deviceID;

	std::vector<Button> buttons;
	std::vector<Field> fields;
	int action;

	Game* game;
	Server* server;
	Client* client;
	std::string ip;

	std::string pseudo;

public:
	#pragma warning(disable:26495)
	Window(const char* title)
		: AbstractWindow(title)
	{

	}
	#pragma warning(default:26495)

private:
	void onSetup() override
	{
		std::srand((unsigned int)std::time(nullptr));

		musicStarted = false;
		deviceID = SDL_OpenAudioDevice(nullptr, 0, &Ressources::wav_spec, nullptr, 0);

		action = 1;

		game = nullptr;
		server = nullptr;
		client = nullptr;
	}

	void setupMenuPseudo()
	{
		SDL_Rect rect = {width / 4, height / 5, width / 2, height / 5};
		buttons.emplace_back(rect, "Entrez votre pseudo");
		buttons[0].action = [&]()
		{
			std::string text = fields[0].getText();

			if (text.size() > 0)
			{
				pseudo = text;
				action = 2;
			}
		};

		rect.y = 3 * height / 5;
		fields.emplace_back(rect);
		fields[0].setFocus(true);
		fields[0].action = [&](std::string text)
		{
			if (text.size() > 0)
			{
				pseudo = text;
				action = 2;
			}
		};
	}

	void setupMenuPrincipal()
	{
		buttons.reserve(2);

		SDL_Rect rect = {width / 4, height / 5, width / 2, height / 5};
		buttons.emplace_back(rect, "Cr�er une partie");
		buttons[0].action = [&]()
		{
			action = 3;
		};

		rect.y = 3 * height / 5;
		buttons.emplace_back(rect, "Rejoindre une partie");
		buttons[1].action = [&]()
		{
			action = 4;
		};
	}

	void setupMenuStart()
	{
		SDL_Rect rect = {width / 4, 2 * height / 5, width / 2, height / 5};
		buttons.emplace_back(rect, "D�marrer");
		buttons[0].action = [&]()
		{
			action = 6;
		};
	}

	void setupMenuClient()
	{
		SDL_Rect rect = {width / 4, 2 * height / 5, width / 2, height / 5};
		fields.emplace_back(rect);
		fields[0].setFocus(true);
		fields[0].action = [&](std::string text)
		{
			action = 5;
			ip = text;
		};

		rect = {10, 10, 200, 50};
		buttons.emplace_back(rect, "retour");
		buttons[0].action = [&]()
		{
			action = 2;
		};
	}

	void updateAction()
	{
		/* menu pseudo */
		if (action == 1)
		{
			buttons.clear();
			fields.clear();
			setupMenuPseudo();
		}
		/* menu principal */
		if (action == 2)
		{
			buttons.clear();
			fields.clear();
			setupMenuPrincipal();
		}
		/* start server */
		else if (action == 3)
		{
			buttons.clear();
			fields.clear();
			setupMenuStart();
			server = new Server();
			server->listen();
			client = new Client("localhost", pseudo);
			client->listen();
			game = &client->game;
		}
		/* open client menu */
		else if (action == 4)
		{
			buttons.clear();
			fields.clear();

			setupMenuClient();
		}
		/* start client */
		else if (action == 5)
		{
			buttons.clear();
			fields.clear();
			client = new Client(ip, pseudo);
			client->listen();
			game = &client->game;
		}
		/* start game */
		else if (action == 6)
		{
			buttons.clear();
			fields.clear();
			server->startGame();
		}

		action = 0;
	}

	void onUpdateEvent(SDL_Event& event) override
	{
		switch (event.type)
		{
			case SDL_QUIT:
				continuer = 0;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						continuer = false;
						break;
					case SDLK_LEFT:
						if (client) client->sendRequest("LEFT", nullptr);
						break;
					case SDLK_RIGHT:
						if (client) client->sendRequest("RIGHT", nullptr);
						break;
					case SDLK_DOWN:
						if (client) client->sendRequest("DOWN", nullptr);
						break;
					case SDLK_UP:
						if (client) client->sendRequest("ROTATE", nullptr);
						break;
				}
				break;
		}

		for (Button& button : buttons)
			button.update(event);

		for (Field& field : fields)
			field.update(event);
	}

	int compteur = 0;
	double lastCompteurTime = -1;

	void onUpdate() override
	{
		compteur++;
		if (timing.time - lastCompteurTime > 1)
		{
			std::cout << compteur << " fps\n";
			lastCompteurTime = timing.time;
			compteur = 0;
		}
		updateAction();

		if (server)
			server->updateGame(timing.time);

		if (client)
		{
			if ((!musicStarted || Ressources::wav_buffer == 0) && client->game.isStarted())
			{
				musicStarted = true;
				SDL_QueueAudio(deviceID, Ressources::wav_buffer, Ressources::wav_length);
				SDL_PauseAudioDevice(deviceID, 0);
			}

			client->getGameFromServer(timing.time);
		}
	}
	
	void onDraw() override
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		if (client)
		{
			SDL_Rect dest = {0, 0, width, height};
			client->game.render(renderer, dest);
		}

		for (Button& button : buttons)
			button.render(renderer, Ressources::font);

		for (Field& field : fields)
			field.render(renderer, Ressources::font);
	}

	void onClose() override
	{
		SDL_CloseAudioDevice(deviceID);

		if (server != nullptr)
			server->shutdown();

		if (client != nullptr)
			client->shutdown();

		if (server != nullptr)
			server->join();

		if (client != nullptr)
			client->join();

		delete server;
		delete client;
	}
};

int main(int argc, char** argv)
{
	Window("Tetris").start();

	return 0;
}