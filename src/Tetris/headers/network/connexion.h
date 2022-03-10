#pragma once

#include <mutex>

#include <SDL_net.h>

#include "better_vector.h"

class Server;

/*
  Socket cote serveur.
  Ecoute un unique client.
*/
class Connexion
{
private:
	std::mutex listeningMutex;
	bool listening;
	std::thread listener;

	Server* server;
	TCPsocket client;
	int numeroClient;
	BetterVector<char> buffer;

public:
	/*
	  client : le socket qui ecoute le client
	  from : le serveur d'ou proviens ce socket
	  numeroClient : correspond a l'indice du
	  client dans la liste server->game.tetris
	*/
	Connexion(TCPsocket client, Server* from, int numeroClient);

	Connexion(const Connexion& other) = delete;

	Connexion(Connexion&& other) noexcept;

	/*
	  Lance la connexion sur son propre thread.
	  Elle peut alors commencer a communiquer avec le client.
	*/
	void listen();

private:
	/*
	  Produit une reponse en fonction de request.
	*/
	void createAnswer(const std::string& request);

public:
	/*
	  Arrete le thread de cette connexion.
	*/
	void shutdown();

	/*
	  Appelle la fonction join du thread de cette connexion.
	*/
	void join();

	/*
	  Renvoie l'IP du client.
	*/
	std::string getIp() const;

private:
	void setListening(bool value);
};
