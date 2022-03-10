#include "fenetre/field.h"

Field::Field(const SDL_Rect& rect)
	: Button(rect)
{
	focus = false;
	done = false;

	Button::action = [&]()
	{
		focus = true;
		done = false;
	};

	Field::action = nullptr;
}

void Field::update(const SDL_Event& event)
{
	Button::update(event);

	switch (event.type)
	{
		case SDL_MOUSEBUTTONUP:
			if (!inside())
				focus = false;
			break;

		case SDL_KEYDOWN:
			if (focus)
			{
				char c = getKeyChar(event.key.keysym.sym);

				/* backspace */
				if (c == 8)
				{
					if (text.size() > 0)
						text.erase(text.size() - 1, 1);
				}
				/* enter */
				else if (c == 10 || c == 13)
				{
					focus = false;
					done = true;
					action(text);
				}
				else if (c != 0)
					text += c;
			}
			break;
	}
}

void Field::render(SDL_Renderer* renderer, TTF_Font* font) const
{
	Button::render(renderer, font);
}

std::string Field::getText() const
{
	return text;
}

void Field::setFocus(bool focus)
{
	this->focus = focus;
}

char Field::getKeyChar(int sym)
{
	if (sym == SDLK_0)
		return '0';
	if (sym == SDLK_1)
		return '1';
	if (sym == SDLK_2)
		return '2';
	if (sym == SDLK_3)
		return '3';
	if (sym == SDLK_4)
		return '4';
	if (sym == SDLK_5)
		return '5';
	if (sym == SDLK_6)
		return '6';
	if (sym == SDLK_7)
		return '7';
	if (sym == SDLK_8)
		return '8';
	if (sym == SDLK_9)
		return '9';

	if (sym == SDLK_KP_0)
		return '0';
	if (sym == SDLK_KP_1)
		return '1';
	if (sym == SDLK_KP_2)
		return '2';
	if (sym == SDLK_KP_3)
		return '3';
	if (sym == SDLK_KP_4)
		return '4';
	if (sym == SDLK_KP_5)
		return '5';
	if (sym == SDLK_KP_6)
		return '6';
	if (sym == SDLK_KP_7)
		return '7';
	if (sym == SDLK_KP_8)
		return '8';
	if (sym == SDLK_KP_9)
		return '9';
	if (sym == SDLK_KP_PERIOD)
		return '.';
	if (sym == SDLK_KP_ENTER)
		return '\n';

	if (sym == SDLK_COLON)
		return ':';
	if (sym == SDLK_SEMICOLON)
		return '.';

	if (sym == SDLK_CAPSLOCK)
		return 0;
	if (sym == SDLK_NUMLOCKCLEAR)
		return 0;
	if (sym == SDLK_RSHIFT)
		return 0;
	if (sym == SDLK_LSHIFT)
		return 0;
	if (sym == SDLK_RCTRL)
		return 0;
	if (sym == SDLK_LCTRL)
		return 0;
	if (sym == SDLK_RALT)
		return 0;
	if (sym == SDLK_LALT)
		return 0;

	return sym;
}