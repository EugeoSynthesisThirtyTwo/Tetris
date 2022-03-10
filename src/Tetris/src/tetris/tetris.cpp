#include "tetris/tetris.h"
#include "network/serializer.h"
#include "fenetre/button.h"
#include "fenetre/ressources.h"

class Game
{
public:
	double getClockSecondes() const;
};

/******************* TetrisEffect **************************/

TetrisEffect::TetrisEffect()
{
	reset();
}

TetrisEffect::TetrisEffect(const char* buffer, int& cursor)
{
	active = unserializeBool(buffer, cursor);
	startTime = unserializeDouble(buffer, cursor);
	duration = unserializeDouble(buffer, cursor);
}

void TetrisEffect::serialize(BetterVector<char>& buffer) const
{
	serializeBool(buffer, active);
	serializeDouble(buffer, startTime);
	serializeDouble(buffer, duration);
}

void TetrisEffect::reset()
{
	active = false;
	startTime = 0;
	duration = 0;
}

/******************* Line **************************/

Line::Line()
{
	clearEffect.reset();
	line = 0;
}

Line::Line(const char* buffer, int& cursor)
{
	clearEffect = std::move(TetrisEffect(buffer, cursor));
	line = unserializeInt(buffer, cursor);
	int size = unserializeInt(buffer, cursor);
	cases.reserve(size);

	for (int i = 0; i < size; i++)
		cases.emplace_back(buffer, cursor);
}

void Line::serialize(BetterVector<char>& buffer) const
{
	clearEffect.serialize(buffer);
	serializeInt(buffer, line);
	serializeInt(buffer, cases.size());

	for (const Color& c : cases)
		c.serialize(buffer);
}

/******************* Tetris **************************/

Tetris::Tetris(int width, int height, Game* from)
	: BasicTetris(width, height)
{
	this->game = from;
}

Tetris::Tetris(Tetris&& other) noexcept
	: BasicTetris(std::move(other))
{
	game = other.game;
	LRReversed = other.LRReversed;
	lines = std::move(other.lines);
}

Tetris::Tetris(const char* buffer, int& cursor, Game* from)
	: BasicTetris(buffer, cursor)
{
	game = from;
	LRReversed = std::move(TetrisEffect(buffer, cursor));

	int size = unserializeInt(buffer, cursor);
	lines.reserve(size);

	for (int i = 0; i < size; i++)
		lines.emplace_back(buffer, cursor);
}

void Tetris::serialize(BetterVector<char>& buffer) const
{
	BasicTetris::serialize(buffer);
	LRReversed.serialize(buffer);

	serializeInt(buffer, lines.size());

	for (const Line& line : lines)
		line.serialize(buffer);
}

void Tetris::operator=(Tetris&& other) noexcept
{
	BasicTetris::operator=(std::move(other));
	LRReversed = other.LRReversed;
	lines = std::move(other.lines);
}

bool Tetris::down()
{
	return BasicTetris::down();
}

bool Tetris::right()
{
	if (LRReversed.active)
		return BasicTetris::left();

	return BasicTetris::right();
}

bool Tetris::left()
{
	if (LRReversed.active)
		return BasicTetris::right();

	return BasicTetris::left();
}

bool Tetris::rotate()
{
	return BasicTetris::rotate();
}

bool Tetris::update()
{
	double clockSecondes = game->getClockSecondes();
	if (clockSecondes - LRReversed.startTime >= LRReversed.duration)
		LRReversed.reset();

	for (size_t i = 0; i < lines.size(); i++)
		if (clockSecondes - lines[i].clearEffect.startTime >= lines[i].clearEffect.duration)
		{
			lines.erase(lines.begin() + i);
			i--;
		}

	return BasicTetris::update(clockSecondes);
}

void Tetris::reverseLeftRight()
{
	LRReversed.active = true;
	LRReversed.startTime = game->getClockSecondes();
	LRReversed.duration = 3;
}

void Tetris::animationLineClear(int y)
{
	Line line;
	line.clearEffect.active = true;
	line.clearEffect.startTime = game->getClockSecondes();
	line.clearEffect.duration = 1;
	line.line = y;

	int width = grid.getWidth();
	line.cases.reserve(width);

	for (int i = 0; i < width; i++)
		line.cases.push_back(grid.at(i, y).color);

	lines.push_back(std::move(line));
}

void Tetris::render(SDL_Renderer* renderer, SDL_Rect dest) const
{
	int heightEffects = dest.h / 10;

	SDL_Rect destBasicTetris = dest;
	destBasicTetris.y = dest.y + heightEffects;
	destBasicTetris.h = dest.h - heightEffects;

	SDL_Rect destEffects = BasicTetris::render(renderer, destBasicTetris);
	destEffects.y = dest.y;
	destEffects.h = heightEffects;
	renderEffects(renderer, destEffects);

	SDL_Rect transitionLineRect;
	SDL_Rect target;
	SDL_Rect from, to;
	int sizeLine;
	double h;

	double clockSecondes = game->getClockSecondes();

	int i = 0;
	for (const Line& line : lines)
	{
		from = grid.getLineRect(line.line, getGridClipRect(destBasicTetris));
		to = from;
		to.x += from.w + 1;
		h = 1 - (clockSecondes - line.clearEffect.startTime) / line.clearEffect.duration;
		h = sqrt(1 - h * h);

		transitionLineRect.x = (int) ((1 - h) * from.x + h * to.x);
		target.y = (int) ((1 - h) * from.y + h * to.y);
		transitionLineRect.w = (int) ((1 - h) * from.w + h * to.w);
		target.h = (int) ((1 - h) * from.h + h * to.h);

		sizeLine = line.cases.size();
		target.w = (int) (transitionLineRect.w / sizeLine - 1);

		for (int x = 0; x < sizeLine; x++)
		{
			Color color = line.cases[x];
			color.a = decltype(color.a) (color.a * (1 - h));
			target.x = (int) (transitionLineRect.x + (double)x / sizeLine * transitionLineRect.w);
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
			SDL_RenderFillRect(renderer, &target);
		}
	}
}

void Tetris::renderEffects(SDL_Renderer* renderer, const SDL_Rect& dest) const
{
	int x_dest = dest.x;

	if (LRReversed.active)
	{
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, Ressources::inversion);
		SDL_Rect clipRect = Ressources::inversion->clip_rect;
		Grid::makeOrthonormal(clipRect, dest);
		clipRect.x = x_dest;
		x_dest += clipRect.w;
		SDL_RenderCopy(renderer, textTexture, nullptr, &clipRect);
		Button::debugTexture(renderer);
		SDL_DestroyTexture(textTexture);
	}
}

/* change le jeu qui contient ce tetris */
void Tetris::setGame(Game* from)
{
	game = from;
}