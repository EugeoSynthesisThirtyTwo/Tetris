#include <iostream>
#include <sstream>
#include <string>

#include "network/server.h"
#include "network/connexion.h"
#include "network/serializer.h"

Connexion::Connexion(TCPsocket client, Server* from, int numeroClient)
{
	setListening(false);

	this->client = client;
	server = from;

	std::string ip = getIp();

	if (ip.empty())
		std::cout << "Nouvelle connexion d'une ip inconnue." << std::endl;
	else
		std::cout << "Nouvelle connexion de " << ip << std::endl;

	this->numeroClient = numeroClient;
}

Connexion::Connexion(Connexion&& other) noexcept
{
	setListening(other.listening);
	listener = std::move(other.listener);
	server = other.server;
	client = other.client;
	numeroClient = other.numeroClient;
}

void Connexion::listen()
{
	listener = std::thread([&]()
	{
		setListening(true);

		while (listening)
		{
			constexpr int requestLength = 256;
			char request[requestLength];
			int length = SDLNet_TCP_Recv(client, request, requestLength);

			if (!length)
			{
				std::cout << "SDLNet_TCP_Recv: " << SDLNet_GetError() << std::endl;
				shutdown();
			}
			else
			{
				if (length >= requestLength)
					request[requestLength - 1] = '\0';
				else
					request[length] = '\0';

				createAnswer(request);
				length = SDLNet_TCP_Send(client, buffer.data(), buffer.size());

				if ((size_t) length < buffer.size())
				{
					std::cout << "SDLNet_TCP_Send: " << SDLNet_GetError() << std::endl;
					shutdown();
				}
			}
		}

		SDLNet_TCP_Close(client);
	});
}

void Connexion::createAnswer(const std::string& request)
{
	buffer.clear();
	serializeInt(buffer, 0);

	if (request == "GET_GAME")
	{
		/* la copie est importante ici car les donnees pourraient etres modifies */
		server->serializeGame(buffer);
	}
	else if (request == "GET_NUMERO_CLIENT")
	{
		serializeInt(buffer, numeroClient);
	}
	else
	{
		std::string answerString;

		if (request == "PING")
		{
			answerString = "PONG";
		}
		else if (request == "LEFT")
		{
			server->performAction([&](std::vector<Tetris>& tetris)
			{
				if (server->isGameStarted())
					tetris[numeroClient].left();
			});

			answerString = "OK";
		}
		else if (request == "RIGHT")
		{
			server->performAction([&](std::vector<Tetris>& tetris)
			{
				if (server->isGameStarted())
				tetris[numeroClient].right();
			});

			answerString = "OK";
		}
		else if (request == "DOWN")
		{
			server->performAction([&](std::vector<Tetris>& tetris)
			{
				if (server->isGameStarted())
					tetris[numeroClient].down();
			});

			answerString = "OK";
		}
		else if (request == "ROTATE")
		{
			server->performAction([&](std::vector<Tetris>& tetris)
			{
				if (server->isGameStarted())
					tetris[numeroClient].rotate();
			});

			answerString = "OK";
		}
		else if (request.rfind("SET_PSEUDO ", 0) == 0)
		{
			server->performAction([&](std::vector<Tetris>& tetris)
			{
				tetris[numeroClient].pseudo = request.substr(std::strlen("SET_PSEUDO "), std::string::npos);
			});

			answerString = "OK";
		}

		for (size_t i = 0; i < answerString.size(); i++)
			buffer.push_back(answerString[i]);

		buffer.push_back('\0');
	}

	BetterVector<char> firstInt;
	serializeInt(firstInt, buffer.size());

	for (size_t i = 0; i < firstInt.size(); i++)
		buffer[i] = firstInt[i];
}

void Connexion::shutdown()
{
	setListening(false);
}

/* attend que le serveur soit éteint */
void Connexion::join()
{
	listener.join();
}

/* get the client IP and port number */
std::string Connexion::getIp() const
{
	IPaddress* remoteip;
	remoteip = SDLNet_TCP_GetPeerAddress(client);

	if (!remoteip)
	{
		std::cout << "SDLNet_TCP_GetPeerAddress: " << SDLNet_GetError() << std::endl;
		return "";
	}

	Uint32 ipaddr;
	ipaddr = SDL_SwapBE32(remoteip->host);

	std::stringstream stream;

	stream << (ipaddr >> 24) << ".";
	stream << ((ipaddr >> 16) & 0xff) << ".";
	stream << ((ipaddr >> 8) & 0xff) << ".";
	stream << (ipaddr & 0xff) << ":";
	stream << remoteip->port;

	return stream.str();
}

void Connexion::setListening(bool value)
{
	listeningMutex.lock();
	listening = value;
	listeningMutex.unlock();
}