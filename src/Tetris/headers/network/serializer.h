#pragma once

#include <string>

#include "better_vector.h"

/*
  unserializeX(buffer, cursor) : renvoie une
  variable de type X lue dans buffer, et decale cursor

  serializeX(buffer, value) : ajoute value, de type X,
  dans buffer.
*/

bool unserializeBool(const char* buffer, int& cursor);

void serializeBool(BetterVector<char>& buffer, bool value);

char unserializeChar(const char* buffer, int& cursor);

void serializeChar(BetterVector<char>& buffer, char value);

int unserializeInt(const char* buffer, int& cursor);

void serializeInt(BetterVector<char>& buffer, int value);

double unserializeDouble(const char* buffer, int& cursor);

void serializeDouble(BetterVector<char>& buffer, double value);

std::string unserializeString(const char* buffer, int& cursor);

void serializeString(BetterVector<char>& buffer, const std::string& value);

SDL_Rect unserializeSDL_Rect(const char* buffer, int& cursor);

void serializeSDL_Rect(BetterVector<char>& buffer, const SDL_Rect& value);