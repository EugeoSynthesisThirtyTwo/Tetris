#include "fenetre/color.h"
#include "network/serializer.h"

Color::Color()
	: r(0), g(0), b(0), a(0)
{

}

Color::Color(Uint8 r, Uint8 g, Uint8 b)
	: r(r), g(g), b(b), a(255)
{

}

Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	: r(r), g(g), b(b), a(a)
{

}

Color::Color(const char* buffer, int& cursor)
{
	r = unserializeChar(buffer, cursor);
	g = unserializeChar(buffer, cursor);
	b = unserializeChar(buffer, cursor);
	a = unserializeChar(buffer, cursor);
}

void Color::serialize(BetterVector<char>& buffer) const
{
	serializeChar(buffer, r);
	serializeChar(buffer, g);
	serializeChar(buffer, b);
	serializeChar(buffer, a);
}