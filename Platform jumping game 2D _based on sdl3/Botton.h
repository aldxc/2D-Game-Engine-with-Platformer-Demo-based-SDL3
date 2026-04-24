#pragma once
#include <string>
#include <functional>
#include <SDL3/SDL.h>
#include <vector>
#include "core/State.h"
#include "Config.h"

class Renderer;

class Botton {
public:
	using ClickCallback = std::function<void()>;
	explicit Botton();
	Botton(SDL_FRect rect, std::string text, SDL_Color color = SDL_Color({ 100, 100, 100, 255 }), int t_size = Config::DEFAULT_TEXT_SIZE, std::vector<ClickCallback> callBacks = {});
	~Botton() = default;

	void clickBottom() const noexcept;
	void render(Renderer& renderer) const noexcept;
	const SDL_FRect& getRect() const noexcept { return m_rect; }
private:
	SDL_FRect m_rect;
    SDL_Color m_color;
	std::string m_text;
	int m_tsize;
	std::vector<ClickCallback> m_clickCallbacks;
};