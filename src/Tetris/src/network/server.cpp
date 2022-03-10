#include <iostream>

#include "network/server.h"
#include "network/connexion.h"

Server::Server()
	: Server(DEFAULT_PORT)
{

}

Server::Server(int port)
{
	setListening(false);

	std::cout << "Starting server..." << std::endl;
	IPaddress ip;

	if (SDLNet_ResolveHost(&ip, NULL, port) == -1)
	{
		std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	server = SDLNet_TCP_Open(&ip);

	if (!server)
	{
		std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	connexions.reserve(16);

	listenCalled = false;
	firstSerialization = true;
	delaySerialization = 0;
}

Server::~Server()
{
	SDLNet_TCP_Close(server);
}

void Server::listen()
{
	if (listenCalled)
		return;

	listenCalled = true;

	listener = std::thread([&]()
	{
		setListening(true);

		int numeroClient = 0;

		while (listening)
		{
			/* try to accept a connection */
			TCPsocket client = SDLNet_TCP_Accept(server);

			if (client)
			{
				if (listening)
				{
					game.addTetris();
					connexions.emplace_back(client, this, numeroClient++);
					connexions.back().listen();
				}
			}
			else
			{
				/* no connection accepted */
				//std::cout << "SDLNet_TCP_Accept: " << SDLNet_GetError() << std::endl;
				SDL_Delay(100);
			}
		}
	});
}

void Server::startGame()
{
	setListening(false);
	game.start();
}

bool Server::isGameStarted()
{
	return game.isStarted();
}

void Server::updateGame(double clockSecondes)
{
	game.update(clockSecondes);
}

void Server::performAction(std::function<void(std::vector<Tetris>& tetris)> action)
{
	game.performAction(action);
}

void Server::shutdown()
{
	setListening(false);

	for (Connexion& connexion : connexions)
		connexion.shutdown();
}

void Server::join()
{
	for (Connexion& connexion : connexions)
		connexion.join();

	connexions.clear();
	listener.join();
}

void Server::serializeGame(BetterVector<char>& buffer)
{
	mutex.lock();
	double clockSecondes = game.getClockSecondes();

	if (firstSerialization)
	{
		firstSerialization = false;
		lastSerialization = clockSecondes - delaySerialization;
	}

	double delay = clockSecondes - lastSerialization;

	if (delay >= delaySerialization)
	{
		if (delay <= 2 * delaySerialization)
			lastSerialization = clockSecondes;
		else
			lastSerialization += 2 * delaySerialization;

		gameSerialized.clear();
		game.serialize(gameSerialized);
	}

	for (char c : gameSerialized)
		buffer.push_back(c);

	mutex.unlock();
}

void Server::setListening(bool value)
{
	listening = value;
}