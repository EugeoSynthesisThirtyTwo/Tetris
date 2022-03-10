#include "fenetre/button.h"
#include "tetris/grid.h"

Button::Button(const SDL_Rect& dest)
	: dest(dest), text(""), colorBackground(DEFAULT_COLOR), colorText(0, 0, 0, 255), action(nullptr)
{
	pressed = false;
	mouseSet = false;
	mouse_x = 0;
	mouse_y = 0;
}

Button::Button(const SDL_Rect& dest, const std::string& text)
	: dest(dest), text(text), colorBackground(DEFAULT_COLOR), colorText(0, 0, 0, 255), action(nullptr)
{
	pressed = false;
	mouseSet = false;
	mouse_x = 0;
	mouse_y = 0;
}

void Button::update(const SDL_Event& event)
{
	mouse_x = event.motion.x;
	mouse_y = event.motion.y;
	mouseSet = true;

	switch (event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
			if (inside())
				pressed = true;
			break;

		case SDL_MOUSEBUTTONUP:
			pressed = false;

			if (inside() && action != nullptr)
				action();

			break;
	}
}

void Button::render(SDL_Renderer* renderer, TTF_Font* font) const
{
	bool bInside = inside();

	if (!mouseSet || (!pressed && !bInside))
	{
		SDL_SetRenderDrawColor(renderer, colorBackground.r, colorBackground.g, colorBackground.b, 127);
		SDL_RenderFillRect(renderer, &dest);
	}
	else if (!pressed)
	{
		SDL_SetRenderDrawColor(renderer, colorBackground.r, colorBackground.g, colorBackground.b, 192);
		SDL_RenderFillRect(renderer, &dest);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, colorBackground.r, colorBackground.g, colorBackground.b, 255);
		SDL_RenderFillRect(renderer, &dest);
	}

	if (text.size() > 0)
	{
		SDL_Color c = *((SDL_Color*) &colorText);
		SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), c);
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_Rect clipRect = textSurface->clip_rect;
		Grid::makeOrthonormal(clipRect, dest);
		SDL_RenderCopy(renderer, textTexture, nullptr, &clipRect);
		debugTexture(renderer);
		SDL_FreeSurface(textSurface);
		SDL_DestroyTexture(textTexture);
	}
}

bool Button::inside() const
{
	return mouseSet && mouse_x >= dest.x && mouse_y >= dest.y && mouse_x < dest.x + dest.w && mouse_y < dest.y + dest.h;
}

void Button::debugTexture(SDL_Renderer* renderer)
{
	SDL_Rect temp = {0, 0, 0, 0};
	SDL_RenderFillRect(renderer, &temp);
}

const Color Button::DEFAULT_COLOR(255, 255, 255, 255);