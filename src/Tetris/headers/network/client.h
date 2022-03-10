#pragma once

#include <mutex>
#include <functional>

#include <SDL_net.h>

#include "tetris/game.h"

/*
  Objet qui va se connecter au serveur.
  Meme l'hote de la partie dois instancier cet objet,
  et se connecter a localhost
*/
class Client
{
private:
	std::string pseudo;

	std::mutex listeningMutex;
	bool listening;
	std::thread listener;

	std::string ip;
	TCPsocket server;

	std::mutex requestMutex;
	bool doRequest;
	std::string request;
	std::function<void(char* buffer, int length)> onReply;
	int numeroClient;

	bool firstGetGame;
	double lastGetGame;
	double delayGetGame;

public:
	/*
	  Modifier cet objet ne sert a rien.
	  Il faut utiliser la fonction sendRequest
	  pour faire des actions sur la partie.
	*/
	Game game;

public:
	/*
	  ip : l'adresse IP du serveur, par exemple "localhost"
	  pseudo : le pseudo du joueur
	*/
	Client(std::string ip, std::string pseudo);

	/*
	  Lance le client sur son propre thread.
	  Il peut alors commencer a communiquer avec le serveur.
	*/
	void listen();

	/*
	  Arrete le thread de ce client.
	*/
	void shutdown();

	/*
	  Appelle la fonction join du thread de ce client.
	*/
	void join();

	/*
	  onReply s'active quand le serveur reponds.
	  onReply ne doit pas contenir d'appels aux sockets.

	  request peut avoir les valeurs suivantes :

	  - "SET_PSEUDO pseudo" defini le pseudo pour le tetris
	  de ce client

	  - "GET_GAME" recupere les informations de la partie
	  en cours et les place dans l'objet game

	  - "GET_NUMERO_CLIENT" recupere le numero du client,
	  qui correspond a l'indice dans la liste tetris de
	  l'objet game

	  - "LEFT" action d'aller a gauche pour le tetris
	  de ce client

	  - "RIGHT" action d'aller a droite pour le tetris
	  de ce client

	  - "DOWN" action d'aller en bas pour le tetris
	  de ce client

	  - "ROTATE" action de tourner la piece pour le tetris
	  de ce client

	  - "PING" demande a ce que le serveur reponde "PONG" ...
	*/
	void sendRequest(const std::string& request, std::function<void(char* answer, int length)> onReply);

	/*
	  Renvoie l'IP du serveur.
	*/
	std::string getIpServer() const;
	
	/*
	  Lance une requete "GET_GAME" au serveur.
	*/
	void getGameFromServer(double clockSecondes);

private:
	void setListening(bool value);
};
