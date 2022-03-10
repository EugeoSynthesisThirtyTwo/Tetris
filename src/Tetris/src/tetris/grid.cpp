#include "tetris/grid.h"
#include "network/serializer.h"

#if _CONTAINER_DEBUG_LEVEL > 0
#include <string>
#endif

Grid::Grid()
	: width(0), height(0), array(nullptr)
{
	onClearLine = nullptr;
}

Grid::Grid(size_t width, size_t height)
	: width(width), height(height)
{
	array = new GridCase[width * height];
	onClearLine = nullptr;
}

Grid::Grid(const char* buffer, int& cursor)
{
	width = unserializeInt(buffer, cursor);
	height = unserializeInt(buffer, cursor);

	size_t size = width * height;
	array = new GridCase[size];

	for (size_t i = 0; i < size; i++)
		array[i] = std::move(GridCase(buffer, cursor));
}

void Grid::serialize(BetterVector<char>& buffer) const
{
	serializeInt(buffer, width);
	serializeInt(buffer, height);
	
	size_t size = width * height;

	for (size_t i = 0; i < size; i++)
		array[i].serialize(buffer);
}

void Grid::operator=(Grid&& other) noexcept
{
	delete[] array;

	array = other.array;
	width = other.width;
	height = other.height;
	onClearLine = std::move(other.onClearLine);

	other.array = nullptr;
	other.width = 0;
	other.height = 0;
	other.onClearLine = nullptr;
}

Grid::~Grid()
{
	delete[] array;
}

void Grid::clear()
{
	size_t length = width * height;

	for (size_t i = 0; i < length; i++)
		array[i] = GridCase();
}

GridCase& Grid::at(size_t x, size_t y)
{
	return (*this)[width * y + x];
}

const GridCase& Grid::at(size_t x, size_t y) const
{
	return (*this)[width * y + x];
}

GridCase& Grid::operator[](size_t index)
{
	#if _CONTAINER_DEBUG_LEVEL > 0
	if (index < 0 || index >= getWidth() * getHeight())
		throw "Index out of bound exception. Index = " + std::to_string(index) + ", size = " + std::to_string(getWidth() * getHeight());
	#endif

	return array[index];
}

const GridCase& Grid::operator[](size_t index) const
{
	#if _CONTAINER_DEBUG_LEVEL > 0
	if (index < 0 || index >= getWidth() * getHeight())
		throw "Index out of bound exception. Index = " + std::to_string(index) + ", size = " + std::to_string(getWidth() * getHeight());
	#endif

	return array[index];
}

bool Grid::collision(const Piece& piece, int offset_x, int offset_y) const
{
	int x, y;

	for (int i = 0; i < 4; i++)
	{
		x = piece.coords_x[i] + offset_x;
		y = piece.coords_y[i] + offset_y;
		
		if (x < 0 || x >= (int)width || y >= (int)height)
			return true;

		if (y >= 0 && at(x, y).filled)
			return true;
	}

	return false;
}

bool Grid::lockPiece(const Piece& piece, int offset_x, int offset_y)
{
	int x, y;

	for (int i = 0; i < 4; i++)
	{
		x = piece.coords_x[i] + offset_x;
		y = piece.coords_y[i] + offset_y;

		if (x < 0 || x >= (int)width || y < 0 || y >= (int)height)
			return false;

		at(x, y) = GridCase(true, piece.color);
	}

	return true;
}

void Grid::clearLines()
{
	size_t x, y, y2;
	bool filled;

	for (y = 0; y < height; y++)
	{
		filled = true;

		for (x = 0; x < width; x++)
			if (!at(x, y).filled)
			{
				filled = false;
				break;
			}

		if (filled)
		{
			if (onClearLine)
				onClearLine(y);

			for (x = 0; x < width; x++)
				for (y2 = y; y2 > 0; y2--)
					at(x, y2) = at(x, y2 - 1);
		}
	}
}

void Grid::makeOrthonormal(SDL_Rect& rect)
{
	makeOrthonormal(rect, width, height);
}

void Grid::makeOrthonormal(SDL_Rect& rect, size_t width, size_t height)
{
	int wReste = rect.w % width;
	int hReste = rect.h % height;

	rect.x += wReste / 2;
	rect.y += hReste / 2;
	rect.w -= wReste;
	rect.h -= hReste;

	if (rect.w / width > rect.h / height)
	{
		int lastW = rect.w;
		rect.w = rect.h * width / height;
		rect.x += (lastW - rect.w) / 2;
	}
	else
	{
		int lastH = rect.h;
		rect.h = rect.w * height / width;
		rect.y += (lastH - rect.h) / 2;
	}
}

/* fait rentrer rect dans into tout en respectant les proportions */
void Grid::makeOrthonormal(SDL_Rect& rect, const SDL_Rect& into)
{
	if ((double)rect.w / into.w > (double)rect.h / into.h)
	{
		rect.h = (int)((double)into.w * rect.h / rect.w);
		rect.y = into.y + (into.h - rect.h) / 2;
		rect.x = into.x;
		rect.w = into.w;
	}
	else
	{
		rect.w = (int)((double)into.h * rect.w / rect.h);
		rect.x = into.x + (into.w - rect.w) / 2;
		rect.y = into.y;
		rect.h = into.h;
	}
}

SDL_Rect Grid::getLineRect(int line, const SDL_Rect& dest) const
{
	return
	{
		dest.x,
		(int) (dest.y + (double)line / height * dest.h),
		dest.w,
		(int) (dest.h / height - 1)
	};
}

void Grid::render(SDL_Renderer* renderer, const SDL_Rect& dest) const
{
	size_t x, y;
	SDL_Rect target;

	target.w = dest.w / width - 1;
	target.h = dest.h / height - 1;

	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			Color color = at(x, y).getTrueColor();

			target.x = (int)(dest.x + (double)x / width * dest.w);
			target.y = (int)(dest.y + (double)y / height * dest.h);

			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
			SDL_RenderFillRect(renderer, &target);
		}
}

size_t Grid::getWidth() const
{
	return width;
}

size_t Grid::getHeight() const
{
	return height;
}