#pragma once

#include <vector>
#include <mutex>
#include <thread>

#include <SDL_net.h>

#include "network/connexion.h"
#include "tetris/game.h"
#include "better_vector.h"

/*
  Contient le socket du serveur,
  la liste des connexions avec les clients,
  et l'objet game le plus important :
  celui qui est envoye a tous les clients.
*/
class Server
{
private:
	bool listenCalled;
	bool listening;
	std::thread listener;

	TCPsocket server;
	std::vector<Connexion> connexions;

	Game game;
	bool gameStarted;

	std::mutex mutex;
	bool firstSerialization;
	double lastSerialization;
	double delaySerialization;
	BetterVector<char> gameSerialized;

public:
	static const int DEFAULT_PORT = 44810;

public:
	/* ecoute sur le port DEFAULT_PORT */
	Server();

	/* ecoute sur le port 'port' */
	Server(int port);

	/*
	  Ferme le socket du serveur.
	  Pensez a appeler 'shutdown' puis 'join' avant
	  d'appeler le destructeur.
	*/
	~Server();

	/*
	  Lance le serveur sur son propre thread.
	  Il peut desormais accepter des clients.
	*/
	void listen();

	/*
	  Lance la partie.
	  Le serveur n'acceptera plus de clients.
	*/
	void startGame();

	/* true si la fonction 'startGame' a ete appelee */
	bool isGameStarted();

	/* appelle la fonction 'update' de game */
	void updateGame(double clockSecondes);

	/* appelle la fonction 'performAction' de game*/
	void performAction(std::function<void(std::vector<Tetris>& tetris)> action);

	/* eteint le serveur */
	void shutdown();

	/* Attend que le serveur soit eteint. */
	void join();

	/* serialize l'objet game dans buffer */
	void serializeGame(BetterVector<char>& buffer);

private:
	void setListening(bool value);
};