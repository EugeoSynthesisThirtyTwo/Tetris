#include <iostream>
#include <string>

#include "network/client.h"
#include "network/server.h"
#include "network/serializer.h"

Client::Client(std::string ip, std::string pseudo)
	: ip(ip), pseudo(pseudo)
{
	setListening(false);

	std::cout << "Starting client..." << std::endl;
	IPaddress ipAddress;

	int indexColon = ip.find(':');
	std::string ipSansPort;
	std::string portString;
	int port;

	if (indexColon == -1)
	{
		ipSansPort = ip;
		port = Server::DEFAULT_PORT;
	}
	else
	{
		ipSansPort = ip.substr(0, indexColon);
		portString = ip.substr(indexColon + 1, std::string::npos);
		port = std::stoi(portString);
	}

	if (SDLNet_ResolveHost(&ipAddress, ipSansPort.c_str(), port) == -1)
	{
		std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	server = SDLNet_TCP_Open(&ipAddress);

	if (!server)
	{
		std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	doRequest = false;
	numeroClient = -1;

	firstGetGame = true;
	delayGetGame = 0;
}

void Client::listen()
{
	listener = std::thread([&]()
	{
		setListening(true);

		while (listening) if (doRequest)
		{
			requestMutex.lock();
			int length = SDLNet_TCP_Send(server, request.c_str(), request.size());

			if ((size_t) length < request.size())
			{
				std::cout << "SDLNet_TCP_Send: " << SDLNet_GetError() << std::endl;
				shutdown();
			}
			else
			{
				constexpr int answerBeginLength = 256;
				char answerBegin[answerBeginLength];
				length = SDLNet_TCP_Recv(server, answerBegin, answerBeginLength);

				if (length <= 0)
				{
					std::cout << "SDLNet_TCP_Recv: " << SDLNet_GetError() << std::endl;
					shutdown();
				}
				else
				{
					int totalLength = length;
					int cursor = 0;
					int expectedTotalLength = unserializeInt(answerBegin, cursor);
					char* answer = new char[expectedTotalLength];
					std::memcpy(answer, answerBegin, length);

					while (totalLength < expectedTotalLength)
					{
						length = SDLNet_TCP_Recv(server, answer + totalLength, expectedTotalLength - totalLength);

						if (length <= 0)
						{
							shutdown();
							break;
						}

						totalLength += length;
					}

					if (listening && onReply != nullptr)
						onReply(answer + sizeof(int), totalLength - sizeof(int));

					delete[] answer;
				}
			}

			doRequest = false;
			requestMutex.unlock();
		}

		SDLNet_TCP_Close(server);
	});

	sendRequest("GET_NUMERO_CLIENT", [&](char* answer, int length)
	{
		int cursor = 0;
		numeroClient = unserializeInt(answer, cursor);
	});

	while (numeroClient == -1)
		SDL_Delay(50);

	sendRequest("SET_PSEUDO " + pseudo, nullptr);
}

void Client::shutdown()
{
	setListening(false);
}

void Client::join()
{
	listener.join();
}

void Client::sendRequest(const std::string& request, std::function<void(char* answer, int length)> onReply)
{
	requestMutex.lock();
	this->request = request;
	this->onReply = onReply;
	this->doRequest = true;
	requestMutex.unlock();
}

std::string Client::getIpServer() const
{
	return ip;
}

void Client::getGameFromServer(double clockSecondes)
{
	if (firstGetGame)
	{
		firstGetGame = false;
		lastGetGame = clockSecondes - delayGetGame;
	}

	if (clockSecondes - lastGetGame >= delayGetGame)
	{
		lastGetGame = clockSecondes;
		std::string request = "GET_GAME";

		sendRequest(request, [&](char* answer, int length)
		{
			int cursor = 0;
			game = std::move(Game(answer, cursor));
		});
	}
}

void Client::setListening(bool value)
{
	listeningMutex.lock();
	listening = value;
	listeningMutex.unlock();
}