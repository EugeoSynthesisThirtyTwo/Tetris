#include <SDL_ttf.h>

#include "tetris/basic_tetris.h"
#include "network/serializer.h"
#include "fenetre/ressources.h"
#include "fenetre/button.h"

BasicTetris::BasicTetris(int width, int height)
	: width(width), height(height), alive(true)
{
	grid = std::move(Grid(width, height));
	offset_x = width / 2;
	offset_y = -1;

	delayUpdate = 0.15;
	firstUpdate = true;
	pseudo = "player";
}

BasicTetris::BasicTetris(BasicTetris&& other) noexcept
{
	operator=(std::move(other));
}

BasicTetris::BasicTetris(const char* buffer, int& cursor)
{
	width = unserializeInt(buffer, cursor);
	height = unserializeInt(buffer, cursor);
	grid = std::move(Grid(buffer, cursor));
	currentPiece = std::move(Piece(buffer, cursor));
	nextPiece = std::move(Piece(buffer, cursor));
	offset_x = unserializeInt(buffer, cursor);
	offset_y = unserializeInt(buffer, cursor);
	alive = unserializeBool(buffer, cursor);
	lastUpdate = unserializeDouble(buffer, cursor);
	delayUpdate = unserializeDouble(buffer, cursor);
	firstUpdate = unserializeBool(buffer, cursor);
	pseudo = unserializeString(buffer, cursor);
}

void BasicTetris::serialize(BetterVector<char>& buffer) const
{
	serializeInt(buffer, width);
	serializeInt(buffer, height);
	grid.serialize(buffer);
	currentPiece.serialize(buffer);
	nextPiece.serialize(buffer);
	serializeInt(buffer, offset_x);
	serializeInt(buffer, offset_y);
	serializeBool(buffer, alive);
	serializeDouble(buffer, lastUpdate);
	serializeDouble(buffer, delayUpdate);
	serializeBool(buffer, firstUpdate);
	serializeString(buffer, pseudo);
}

void BasicTetris::operator=(BasicTetris&& other) noexcept
{
	width = other.width;
	height = other.height;
	alive = other.alive;

	grid = std::move(other.grid);
	currentPiece = other.currentPiece;
	nextPiece = other.nextPiece;
	offset_x = other.offset_x;
	offset_y = other.offset_y;

	other.width = 0;
	other.height = 0;
	other.alive = false;

	lastUpdate = other.lastUpdate;
	delayUpdate = other.delayUpdate;
	firstUpdate = other.firstUpdate;

	pseudo = std::move(other.pseudo);
}

bool BasicTetris::down()
{
	offset_y++;

	if (grid.collision(currentPiece, offset_x, offset_y))
	{
		if (grid.lockPiece(currentPiece, offset_x, offset_y - 1))
		{
			grid.clearLines();
			offset_x = width / 2;
			offset_y = -1;

			currentPiece = nextPiece;
			nextPiece.randomize();
		}
		else
			alive = false;

		return false;
	}

	return true;
}

bool BasicTetris::right()
{
	offset_x++;

	if (grid.collision(currentPiece, offset_x, offset_y))
	{
		offset_x--;
		return false;
	}

	return true;
}

bool BasicTetris::left()
{
	offset_x--;

	if (grid.collision(currentPiece, offset_x, offset_y))
	{
		offset_x++;
		return false;
	}

	return true;
}

bool BasicTetris::rotate()
{
	currentPiece.rotate(false);

	if (grid.collision(currentPiece, offset_x, offset_y))
	{
		if (!right() && !left())
		{
			currentPiece.rotate(true);
			return false;
		}
	}

	return true;
}

bool BasicTetris::isAlive() const
{
	return alive;
}

bool BasicTetris::update(double clockSecondes)
{
	if (firstUpdate)
	{
		lastUpdate = clockSecondes - delayUpdate;
		firstUpdate = false;
	}

	if (clockSecondes - lastUpdate >= delayUpdate)
	{
		down();
		lastUpdate += delayUpdate;
		return true;
	}

	return false;
}

void BasicTetris::setOnClearLine(std::function<void(int y)> onClearLine)
{
	grid.onClearLine = onClearLine;
}

void BasicTetris::renderPiece(SDL_Renderer* renderer, const SDL_Rect& dest, const Piece& piece, int offset_x, int offset_y) const
{
	SDL_Rect target;
	target.w = dest.w / width - 1;
	target.h = dest.h / height - 1;
	Color color = piece.color;
	int x, y;

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	for (int i = 0; i < 4; i++)
	{
		x = piece.coords_x[i] + offset_x;
		y = piece.coords_y[i] + offset_y;

		target.x = (int)(dest.x + (double)x / width * dest.w);
		target.y = (int)(dest.y + (double)y / height * dest.h);

		SDL_RenderFillRect(renderer, &target);
	}
}

SDL_Rect BasicTetris::render(SDL_Renderer* renderer, SDL_Rect dest) const
{
	Grid::makeOrthonormal(dest, width + paddingRight, height);

	int dx = dest.w / (width + paddingRight);
	SDL_Rect gridDest = dest;
	gridDest.w -= paddingRight * dx;
	grid.render(renderer, gridDest);

	SDL_Rect pseudoDest = dest;
	pseudoDest.x = gridDest.x + gridDest.w + dx;
	pseudoDest.w = dest.w - pseudoDest.x + dest.x;
	pseudoDest.y = dest.y + dest.h - 3 * dx;
	pseudoDest.h = dest.h - pseudoDest.y + dest.y;
	renderPseudo(renderer, pseudoDest);

	renderPiece(renderer, gridDest, currentPiece, offset_x, offset_y);
	renderPiece(renderer, gridDest, nextPiece, width + 2, height / 2 - 1);

	return dest;
}

SDL_Rect BasicTetris::getGridClipRect(SDL_Rect dest) const
{
	Grid::makeOrthonormal(dest, width + paddingRight, height);
	dest.w -= paddingRight * dest.w / (width + paddingRight);
	return dest;
}

void BasicTetris::renderPseudo(SDL_Renderer* renderer, const SDL_Rect& dest) const
{
	if (pseudo.size() > 0)
	{
		SDL_Color color = {255, 255, 255, 255};
		SDL_Surface* surface = TTF_RenderText_Blended(Ressources::font, pseudo.c_str(), color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_Rect clipRect = surface->clip_rect;
		Grid::makeOrthonormal(clipRect, dest);
		SDL_RenderCopy(renderer, texture, nullptr, &clipRect);
		Button::debugTexture(renderer);
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}
}
