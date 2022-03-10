#include "tetris/gridcase.h"
#include "network/serializer.h"

GridCase::GridCase()
	: filled(false), color(DEFAULT_COLOR)
{

}

GridCase::GridCase(bool filled, Color color)
	: filled(filled), color(color)
{

}

GridCase::GridCase(const char* buffer, int& cursor)
{
	filled = unserializeBool(buffer, cursor);
	color = std::move(Color(buffer, cursor));
}

void GridCase::serialize(BetterVector<char>& buffer) const
{
	serializeBool(buffer, filled);
	color.serialize(buffer);
}

Color GridCase::getTrueColor() const
{
	if (filled)
		return color;

	return DEFAULT_COLOR;
}

Color GridCase::DEFAULT_COLOR(255, 255, 255, 63);