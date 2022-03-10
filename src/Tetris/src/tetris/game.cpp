#include "tetris/tetris.h"
#include "tetris/game.h"
#include "network/serializer.h"

Game::Game()
{
	clockSecondes = 0;
	started = false;
}

Game::Game(const char* buffer, int& cursor)
{
	clockSecondes = unserializeDouble(buffer, cursor);
	started = unserializeBool(buffer, cursor);
	int size = unserializeInt(buffer, cursor);
	tetris.reserve(size);

	for (int i = 0; i < size; i++)
		tetris.emplace_back(buffer, cursor, this);
}

void Game::serialize(BetterVector<char>& buffer) const
{
	mutex.lock();

	serializeDouble(buffer, clockSecondes);
	serializeBool(buffer, started);
	serializeInt(buffer, tetris.size());

	for (size_t i = 0; i < tetris.size(); i++)
		tetris[i].serialize(buffer);

	mutex.unlock();
}

void Game::operator=(Game&& other) noexcept
{
	mutex.lock();
	other.mutex.lock();

	tetris = std::move(other.tetris);

	for (Tetris& t : tetris)
		t.setGame(this);

	clockSecondes = other.clockSecondes;
	started = other.started;

	other.mutex.unlock();
	mutex.unlock();
}

void Game::addTetris()
{
	mutex.lock();

	tetris.emplace_back(Game::DEFAULT_WIDTH, Game::DEFAULT_HEIGHT, this);
	setupOnClear(tetris.size() - 1);

	mutex.unlock();
}

void Game::update(double clockSecondes)
{
	mutex.lock();
	this->clockSecondes = clockSecondes;

	if (started)
	{
		int nbTetris = tetris.size();

		for (int i = 0; i < nbTetris; i++)
		{
			if (tetris[i].isAlive())
				tetris[i].update();
			else
			{
				std::string pseudo = tetris[i].pseudo;
				tetris[i] = std::move(Tetris(Game::DEFAULT_WIDTH, Game::DEFAULT_HEIGHT, this));
				tetris[i].pseudo = pseudo;
				setupOnClear(i);
			}
		}
	}

	mutex.unlock();
}

void Game::performAction(std::function<void(std::vector<Tetris>& tetris)> action)
{
	if (action != nullptr)
	{
		mutex.lock();
		action(tetris);
		mutex.unlock();
	}
}

void Game::render(SDL_Renderer* renderer, SDL_Rect dest) const
{
	mutex.lock();

	this->clockSecondes = clockSecondes;
	int nbJoueurs = tetris.size();

	if (nbJoueurs == 0)
	{
		mutex.unlock();
		return;
	}

	SDL_Rect destTetris = dest;
	destTetris.w = dest.w / nbJoueurs;

	for (int i = 0; i < nbJoueurs; i++)
	{
		destTetris.x = dest.x + i * dest.w / nbJoueurs;
		tetris[i].render(renderer, destTetris);
	}

	mutex.unlock();
}

double Game::getClockSecondes() const
{
	return clockSecondes;
}

void Game::start()
{
	started = true;
}

bool Game::isStarted() const
{
	return started;
}

void Game::setupOnClear(int index)
{
	tetris[index].setOnClearLine([&, index](int y)
	{
		tetris[index].animationLineClear(y);

		for (size_t i = 0; i < tetris.size(); i++)
			if (i != index)
				tetris[i].reverseLeftRight();
	});
}