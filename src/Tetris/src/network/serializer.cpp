#include <SDL.h>

#include "network/serializer.h"

bool unserializeBool(const char* buffer, int& cursor)
{
	return buffer[cursor++];
}

void serializeBool(BetterVector<char>& buffer, bool value)
{
	buffer.push_back(value);
}

char unserializeChar(const char* buffer, int& cursor)
{
	return buffer[cursor++];
}

void serializeChar(BetterVector<char>& buffer, char value)
{
	buffer.push_back(value);
}

int unserializeInt(const char* buffer, int& cursor)
{
	int result;
	char* pointer = (char*)&result;

	for (char i = 0; i < sizeof(int); i++)
		pointer[i] = buffer[cursor++];

	return result;
}

void serializeInt(BetterVector<char>& buffer, int value)
{
	char* pointer = (char*)&value;

	for (char i = 0; i < sizeof(int); i++)
		buffer.push_back(pointer[i]);
}

double unserializeDouble(const char* buffer, int& cursor)
{
	double result;
	char* pointer = (char*)&result;

	for (char i = 0; i < sizeof(double); i++)
		pointer[i] = buffer[cursor++];

	return result;
}

void serializeDouble(BetterVector<char>& buffer, double value)
{
	char* pointer = (char*)&value;

	for (char i = 0; i < sizeof(double); i++)
		buffer.push_back(pointer[i]);
}

std::string unserializeString(const char* buffer, int& cursor)
{
	int size = unserializeInt(buffer, cursor);

	std::string result;
	result.resize(size);

	for (int i = 0; i < size; i++)
		result[i] = unserializeChar(buffer, cursor);

	return result;
}

void serializeString(BetterVector<char>& buffer, const std::string& value)
{
	serializeInt(buffer, value.size());
	
	for (char c : value)
		serializeChar(buffer, c);
}

SDL_Rect unserializeSDL_Rect(const char* buffer, int& cursor)
{
	SDL_Rect result;

	result.x = unserializeInt(buffer, cursor);
	result.y = unserializeInt(buffer, cursor);
	result.w = unserializeInt(buffer, cursor);
	result.h = unserializeInt(buffer, cursor);

	return result;
}

void serializeSDL_Rect(BetterVector<char>& buffer, const SDL_Rect& value)
{
	serializeInt(buffer, value.x);
	serializeInt(buffer, value.y);
	serializeInt(buffer, value.w);
	serializeInt(buffer, value.h);
}